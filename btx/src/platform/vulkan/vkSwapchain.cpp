#include "brasstacks/platform/vulkan/vkSwapchain.hpp"

#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/system/window/TargetWindow.hpp"
#include "brasstacks/config/RenderConfig.hpp"
#include "brasstacks/platform/vulkan/devices/vkLogicalDevice.hpp"
#include "brasstacks/platform/vulkan/resources/vkImageObject.hpp"

namespace btx {

// =============================================================================
vkSwapchain::vkSwapchain(vkPhysicalDevice const &adapter,
                         vkLogicalDevice const &device) :
    _render_area  { },
    _image_format { },
    _color_space  { },
    _present_mode { },
    _handle       { nullptr },
    _images       { },
    _adapter      { adapter },
    _device       { device }
{
    _query_surface_capabilities();
    _query_surface_format();
    _query_surface_present_modes();

    vk::SwapchainCreateInfoKHR create_info { };
    _populate_create_info(create_info);

    auto const result = _device.native().createSwapchainKHR(create_info);
    if(result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to create swapchain.");
        return;
    }

    _handle = result.value;
    BTX_TRACE("Created swapchain {:#x}",
              reinterpret_cast<uint64_t>(VkSwapchainKHR(_handle)));

    _get_images();
}

vkSwapchain::~vkSwapchain() {
    for(auto &image : _images) {
        delete image;
    }

    BTX_TRACE(
        "Destroying swapchain {:#x}",
        reinterpret_cast<uint64_t>(VkSwapchainKHR(_handle))
    );

    _device.native().destroy(_handle);
}

// =============================================================================
void vkSwapchain::_query_surface_capabilities() {
    auto const &gpu       = _adapter.native();
    auto const &surface   = TargetWindow::surface();
    auto const cap_result = gpu.getSurfaceCapabilitiesKHR(surface);

    if(cap_result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Could not get surface capabilities.");
        return;
    }

    auto const &capabilities = cap_result.value;
    BTX_TRACE(
        "\nSurface Capabilities:"
        "\n\t Minimum Image Count: {}"
        "\n\t Maximum Image Count: {}"
        "\n\t Current Extent: {} x {}"
        "\n\t Minimum Extent: {} x {}"
        "\n\t Maximum Extent: {} x {}"
        "\n\t Maximum Image Array Layers: {}",
        capabilities.minImageCount,
        capabilities.maxImageCount,
        capabilities.currentExtent.width,
        capabilities.currentExtent.height,
        capabilities.minImageExtent.width,
        capabilities.minImageExtent.height,
        capabilities.maxImageExtent.width,
        capabilities.maxImageExtent.height,
        capabilities.maxImageArrayLayers
    );

    // The vk::SurfaceCapabilitiesKHR struct will let us know what resolutions
    // our surface is allowed to be.
    _render_area.extent.width = std::clamp(
        static_cast<uint32_t>(RenderConfig::window_width),
        capabilities.minImageExtent.width,
        capabilities.maxImageExtent.width
    );
    _render_area.extent.height = std::clamp(
        static_cast<uint32_t>(RenderConfig::window_height),
        capabilities.minImageExtent.height,
        capabilities.maxImageExtent.height
    );

    if(_render_area.extent.width != RenderConfig::window_width ||
       _render_area.extent.height != RenderConfig::window_height)
    {
        BTX_WARN(
            "Requested resolution {}x{} unsupported; using {}x{} instead",
            RenderConfig::window_width, RenderConfig::window_height,
            _render_area.extent.width, _render_area.extent.height
        );
    }

    // Provided image count has already been used to set some array sizes (in
    // LogicalDevice, for example) it's become a hard requirement of the
    // surface itself
    if(RenderConfig::swapchain_image_count > capabilities.maxImageCount ||
       RenderConfig::swapchain_image_count < capabilities.minImageCount)
    {
        BTX_WARN(
            "{} swapchain images requested, but surface allows a minimum of {} "
            "and a maximum of {} images. Defaulting to minimum.",
            RenderConfig::swapchain_image_count,
            capabilities.minImageCount,
            capabilities.maxImageCount
        );

        RenderConfig::swapchain_image_count = capabilities.minImageCount;
    }

    if(RenderConfig::swapchain_image_count == 2 && RenderConfig::vsync_on) {
        if(capabilities.maxImageCount >= 3) {
            RenderConfig::swapchain_image_count = 3;
        }
    }
}

// =============================================================================
void vkSwapchain::_query_surface_format() {
    auto const& gpu = _adapter.native();
    auto const& surface = TargetWindow::surface();
    auto const formats_result = gpu.getSurfaceFormatsKHR(surface);

    if(formats_result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Could not get surface formats.");
        return;
    }

    auto const &formats = formats_result.value;
    BTX_TRACE("Found {} surface formats.", formats.size());

    bool found_desired = false;
    auto const desired_format = vk::Format::eB8G8R8A8Unorm;
    auto const deisred_space = vk::ColorSpaceKHR::eSrgbNonlinear;

    for(auto const& format : formats) {
        if(format.format == desired_format &&
           format.colorSpace == deisred_space)
        {
            _image_format = format.format;
            _color_space  = format.colorSpace;
            found_desired = true;
        }

        BTX_TRACE(
            "    {} / {}",
            vk::to_string(format.format),
            vk::to_string(format.colorSpace)
        );
    }

    if(!found_desired) {
        _image_format = formats[0].format;
        _color_space  = formats[0].colorSpace;

        BTX_WARN(
            "Could not find desired swapchain surface format/color space of "
            "{} / {}. Defaulting instead to {} / {}.",
            vk::to_string(desired_format),
            vk::to_string(deisred_space),
            vk::to_string(_image_format),
            vk::to_string(_color_space)
        );
    }
}

// =============================================================================
void vkSwapchain::_query_surface_present_modes() {
    auto const& gpu = _adapter.native();
    auto const& surface = TargetWindow::surface();
    auto const modes_result = gpu.getSurfacePresentModesKHR(surface);

    if(modes_result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Could not query surface present modes.");
        return;
    }

    auto const &modes = modes_result.value;
    BTX_TRACE("Found {} present modes.", modes.size());

    bool has_fifo_relaxed = false;
    bool has_fifo         = false;
    bool has_immediate    = false;

    // iterate available modes, noting what we've got
    for(auto const mode : modes) {
        BTX_TRACE("    {}", vk::to_string(mode));
        if(mode == vk::PresentModeKHR::eFifoRelaxed) {
            has_fifo_relaxed = true;
        }
        else if(mode == vk::PresentModeKHR::eFifo) {
            has_fifo = true;
        }
        else if(mode == vk::PresentModeKHR::eImmediate) {
            has_immediate = true;
        }
    }

    if(has_fifo_relaxed && RenderConfig::vsync_on) {
        _present_mode = vk::PresentModeKHR::eFifoRelaxed;
    }
    else if(has_fifo && RenderConfig::vsync_on) {
        _present_mode = vk::PresentModeKHR::eFifo;
    }
    else if(RenderConfig::vsync_on) {
        BTX_WARN("VSync requested but the available present modes don't "
                     "support it.");
    }
    else if(has_immediate) {
        _present_mode = vk::PresentModeKHR::eImmediate;
    }
    else {
        BTX_CRITICAL("Neither immediate nor FIFO presentation modes are "
                         "supported.");
    }
}

// =============================================================================
void vkSwapchain::_populate_create_info(vk::SwapchainCreateInfoKHR &create_info) {
    create_info = {
        .surface         = TargetWindow::surface(),
        .minImageCount   = RenderConfig::swapchain_image_count,
        .imageFormat     = _image_format,
        .imageColorSpace = _color_space,
        .imageExtent     = _render_area.extent,

        // Image array layers will always be one, except in the case of a
        // device with displays interested in the same swapchain, like a VR
        // headset
        .imageArrayLayers = 1u,

        // Marking the images in this swapchain as color attachments means they
        // can be used to draw the scene into
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,

        // We don't need to worry about sharing images between queues
        .imageSharingMode = vk::SharingMode::eExclusive,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,

        // Transforms are what you'd expect: the image can be flipped, rotated,
        // etc. Leaving it alone gets us what we want for now.
        .preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity,

        // Working with transparency and blending will come later
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,

        // Store the selected presentation mode
        .presentMode = _present_mode,

        // Setting clipped to true allows Vulkan to ignore pixels on the render
        // surface that can't be seen. For example, moving a window in front
        // of the render surface.
        .clipped = 1u,

        // There are situations during which you'll want to recreate the
        // swapchain from scratch. Providing the previous swapchain aids this
        // process.
        .oldSwapchain = nullptr,
    };

    BTX_INFO(
        "\nSwapchain Create Info:"
        "\n    Extent:       {}x{}"
        "\n    Offset:       {}x{}"
        "\n    Image Count:  {}"
        "\n    Format:       {}"
        "\n    Color Space:  {}"
        "\n    Present Mode: {}",
        _render_area.extent.width, _render_area.extent.height,
        _render_area.offset.x, _render_area.offset.y,
        RenderConfig::swapchain_image_count,
        vk::to_string(_image_format),
        vk::to_string(_color_space),
        vk::to_string(_present_mode)
    );
}

// =============================================================================
void vkSwapchain::_get_images() {
    auto const images_result =
        _device.native().getSwapchainImagesKHR(_handle);

    if(images_result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Could not get swapchain images.");
        return;
    }

    auto const &images = images_result.value;

    if(images.size() != RenderConfig::swapchain_image_count) {
        BTX_CRITICAL(
            "Swapchain supports {} images; {} configured",
            _images.size(),
            RenderConfig::swapchain_image_count
        );
        return;
    }

    BTX_INFO("Acquired {} swapchain images", images.size());

    _images.reserve(RenderConfig::swapchain_image_count);

    for(auto const &image : images) {
        _images.emplace_back(new vkImageObject(
            _device,
            image,
            _image_format,
            vk::ImageLayout::eUndefined,
            vk::ImageAspectFlagBits::eColor
        ));

        _images.back()->create_view(vk::ImageViewType::e2D);
    }
}

} // namespace btx