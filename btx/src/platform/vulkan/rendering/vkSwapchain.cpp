#include "brasstacks/platform/vulkan/rendering/vkSwapchain.hpp"

#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/rendering/vkSurface.hpp"
#include "brasstacks/system/TargetWindow.hpp"
#include "brasstacks/config/RenderConfig.hpp"
#include "brasstacks/platform/vulkan/resources/vkImage.hpp"
#include "brasstacks/platform/vulkan/resources/vkImageView.hpp"
#include "brasstacks/platform/vulkan/rendering/vkFrame.hpp"

namespace btx {

// =============================================================================
uint32_t vkSwapchain::acquire_next_image_index(vk::Semaphore const &semaphore) {
    // Request the next image index, and provide the semaphore we just popped
    auto const result = _device.native().acquireNextImageKHR(
        _handle,
        std::numeric_limits<uint64_t>::max(),
        semaphore,
        VK_NULL_HANDLE,
        &_next_image_index
    );

    if(result != vk::Result::eSuccess) {
        if(result == vk::Result::eSuboptimalKHR ||
           result == vk::Result::eErrorOutOfDateKHR)
        {
            BTX_ERROR("Recreate swapchain: {}", vk::to_string(result));
        }
        else {
            BTX_CRITICAL("Could not acquire next swapchain image: '{}",
                         vk::to_string(result));
        }
    }

    return _next_image_index;
}

// =============================================================================
void vkSwapchain::present(vkFrame const &frame) {
    // This present call will wait on frame.cmds_complete_sem to ensure the
    // submitted batch of commands has finished
    vk::PresentInfoKHR const present_info {
        .waitSemaphoreCount = 1u,
        .pWaitSemaphores = &frame.cmds_complete_semaphore(),
        .swapchainCount  = 1u,
        .pSwapchains     = &_handle,
        .pImageIndices   = &_next_image_index,
    };

    auto const result = _device.cmd_queue().native().presentKHR(present_info);

    if(result != vk::Result::eSuccess) {
        if(result == vk::Result::eSuboptimalKHR ||
           result == vk::Result::eErrorOutOfDateKHR)
        {
            BTX_CRITICAL("Recreate swapchain: {}", vk::to_string(result));
        }
        else {
            BTX_CRITICAL("Failed to submit swapchain image {}: '{}",
                         _next_image_index, vk::to_string(result));
        }
    }
}

// =============================================================================
vkSwapchain::vkSwapchain(vkPhysicalDevice const &adapter,
                         vkSurface const &surface,
                         vkDevice const &device) :

    _adapter      { adapter },
    _surface      { surface },
    _device       { device },
    _render_area  { },
    _image_format { },
    _color_space  { },
    _present_mode { },
    _handle       { nullptr },
    _image_views  { }
{
    _query_surface_capabilities();
    _query_surface_format();
    _query_surface_present_modes();

    vk::SwapchainCreateInfoKHR create_info { };
    _populate_create_info(create_info);

    // Finally, create the swapchain
    auto const result = _device.native().createSwapchainKHR(create_info);
    if(result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to create swapchain: '{}'",
                     vk::to_string(result.result));
        return;
    }

    _handle = result.value;
    BTX_TRACE("Created swapchain {:#x}",
              reinterpret_cast<uint64_t>(::VkSwapchainKHR(_handle)));

    // Now that we've got a swapchain, we want to be able to interact with the
    // images it can present
    _get_swapchain_images();
}

vkSwapchain::~vkSwapchain() {
    for(auto *view : _image_views) {
        delete view;
    }

    BTX_TRACE("Destroying swapchain {:#x}",
              reinterpret_cast<uint64_t>(::VkSwapchainKHR(_handle)));

    _device.native().destroy(_handle);
}

// =============================================================================
void vkSwapchain::_query_surface_capabilities() {
    auto const result =
        _adapter.native().getSurfaceCapabilitiesKHR(_surface.native());

    if(result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Could not get surface capabilities: '{}'",
                     vk::to_string(result.result));
        return;
    }

    auto const &capabilities = result.value;
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

    _render_area.extent = capabilities.currentExtent;

    if(RenderConfig::vsync_on) {
        _image_views.resize(3);
    }
    else {
        _image_views.resize(capabilities.minImageCount);
    }
}

// =============================================================================
void vkSwapchain::_query_surface_format() {
    auto const result =
        _adapter.native().getSurfaceFormatsKHR(_surface.native());

    if(result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Could not get surface formats: '{}'",
                      vk::to_string(result.result));
        return;
    }

    auto const &formats = result.value;
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
    auto const result =
        _adapter.native().getSurfacePresentModesKHR(_surface.native());

    if(result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Could not query surface present modes: '{}'",
                     vk::to_string(result.result));
        return;
    }

    auto const &modes = result.value;
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
        .surface         = _surface.native(),
        .minImageCount   = static_cast<uint32_t>(_image_views.size()),
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

    BTX_TRACE(
        "\nSwapchain Create Info:"
        "\n    Extent:       {}x{}"
        "\n    Offset:       {}x{}"
        "\n    Image Count:  {}"
        "\n    Format:       {}"
        "\n    Color Space:  {}"
        "\n    Present Mode: {}",
        _render_area.extent.width, _render_area.extent.height,
        _render_area.offset.x, _render_area.offset.y,
        _image_views.size(),
        vk::to_string(_image_format),
        vk::to_string(_color_space),
        vk::to_string(_present_mode)
    );
}

// =============================================================================
void vkSwapchain::_get_swapchain_images() {
    auto const result = _device.native().getSwapchainImagesKHR(_handle);
    if(result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Could not get swapchain images: '{}'",
                     vk::to_string(result.result));
        return;
    }

    auto const &swapchain_images = result.value;
    BTX_TRACE("Acquired {} swapchain images", swapchain_images.size());

    if(swapchain_images.size() != _image_views.size()) {
        BTX_CRITICAL("Swapchain provided {} images, expected {}",
                     swapchain_images.size(), _image_views.size());
        return;
    }

    for(uint32_t i = 0u; i < _image_views.size(); ++i) {
        _image_views[i] = new vkImageView(
            _device,
            swapchain_images[i],
            _image_format,
            vk::ImageViewType::e2D,
            vk::ImageAspectFlagBits::eColor
        );
    }
}

} // namespace btx