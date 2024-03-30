#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/swapchain/vkSwapchain.hpp"

#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkQueue.hpp"
#include "brasstacks/platform/vulkan/swapchain/vkSurface.hpp"
#include "brasstacks/config/RenderConfig.hpp"
#include "brasstacks/platform/vulkan/swapchain/vkFrameSync.hpp"

namespace btx {

// =============================================================================
vkSwapchain::vkSwapchain() :
    _handle       { nullptr },
    _device       { nullptr },
    _image_format { vk::Format::eUndefined },
    _present_mode { vk::PresentModeKHR::eImmediate },
    _images       { },
    _image_views  { },
    _size         { 0u, 0u },
    _offset       { 0, 0 },
    _aspect_ratio { 0.0f }
{ }

// =============================================================================
vkSwapchain::~vkSwapchain() {
    if(_handle != nullptr) {
        destroy();
    }
}

// =============================================================================
void vkSwapchain::create(vkSurface const &surface) {
    if(_handle != nullptr) {
        BTX_CRITICAL("Swapchain {} already exists", _handle);
    }

    _device = Renderer::device().native();

    // Grab the supported image counts, resolutions, etc from the surface
    _query_surface_capabilities(surface.native());

    // This swapchain's images will have the same formatting as the provided
    // surface
    _query_surface_format(surface.native());

    // The surface also determines the presentation engine modes we can choose
    // from
    _query_surface_present_modes(surface.native());

    // Build out the create info struct
    auto const create_info = _populate_create_info(surface.native());

    // Finally, create the swapchain
    _handle = _device.createSwapchainKHR(create_info);
    BTX_TRACE("Created swapchain {}", _handle);

    // Now that we've got the swapchain itself, we'll need its images
    _get_swapchain_images();
}

// =============================================================================
void vkSwapchain::destroy() {
    for(auto &image : _images) {
        image->destroy();
    }

    for(auto &view : _image_views) {
        view->destroy();
    }

    BTX_TRACE("Destroying swapchain {}", _handle);
    _device.destroy(_handle);
    _handle = nullptr;
}

// =============================================================================
uint32_t vkSwapchain::get_next_image_index(vk::Semaphore const &semaphore) {
    uint32_t next_image_index;

    // Wait for no more than one second
    using namespace std::chrono_literals;
    static auto const wait_period =
        std::chrono::duration_cast<std::chrono::nanoseconds>(1.0s).count();

    // Request the next image index and signal the provided semaphore when the
    // acquisition is complete
    auto const result = _device.acquireNextImageKHR(
        _handle,          // The swapchain we're trying to get an image from
        wait_period,      // How long to wait for a new image
        semaphore,        // A semaphore to signal when an image is released
        VK_NULL_HANDLE,   // A fence to signal when an image is released
        &next_image_index // The index of the next free image
    );

    if(result != vk::Result::eSuccess) {
        if(result != vk::Result::eSuboptimalKHR &&
           result != vk::Result::eErrorOutOfDateKHR)
        {
            BTX_CRITICAL("Could not acquire next swapchain image: '{}'",
                         vk::to_string(result));
        }

        return std::numeric_limits<uint32_t>::max();
    }

    // Return the index of the image that'll be safe to write next
    return next_image_index;
}

// =============================================================================
bool vkSwapchain::present(vkFrameSync const &frame, uint32_t const image_index)
{
    // This present call will wait on frame.queue_sem to ensure the
    // submitted batch of commands has finished
    vk::PresentInfoKHR const present_info {
        .waitSemaphoreCount = 1u,
        .pWaitSemaphores = &frame.queue_semaphore(),
        .swapchainCount  = 1u,
        .pSwapchains     = &_handle,
        .pImageIndices   = &image_index,
    };

    static vk::Result result = vk::Result::eSuccess;

    try {
        auto const queue = Renderer::device().graphics_queue().native();
        result = queue.presentKHR(present_info);
    } catch([[maybe_unused]] vk::OutOfDateKHRError const &exception) {
        return false;
    }

    if(result == vk::Result::eSuboptimalKHR) {
        return false;
    }

    return true;
}

// =============================================================================
void vkSwapchain::_query_surface_capabilities(vk::SurfaceKHR const &surface) {
    auto const caps =
        vkPhysicalDevice::native().getSurfaceCapabilitiesKHR(surface);

    BTX_TRACE(
        "\nSurface Capabilities:"
        "\n\t Minimum Image Count: {}"
        "\n\t Maximum Image Count: {}"
        "\n\t Current Extent: {} x {}"
        "\n\t Minimum Extent: {} x {}"
        "\n\t Maximum Extent: {} x {}"
        "\n\t Maximum Image Array Layers: {}",
        caps.minImageCount,
        caps.maxImageCount,
        caps.currentExtent.width,
        caps.currentExtent.height,
        caps.minImageExtent.width,
        caps.minImageExtent.height,
        caps.maxImageExtent.width,
        caps.maxImageExtent.height,
        caps.maxImageArrayLayers
    );

    if(caps.currentExtent.width == 0u || caps.currentExtent.height == 0u
       || caps.minImageExtent.width == 0u || caps.minImageExtent.height == 0u
       || caps.maxImageExtent.width == 0u || caps.maxImageExtent.height == 0u)
    {
        BTX_CRITICAL("Cannot create swapchain with zero width or height.");
        return;
    }

    // We intend to draw to the whole surface
    _size.width = caps.currentExtent.width;
    _size.height = caps.currentExtent.height;

    if(_size.width < caps.minImageExtent.width) {
        BTX_WARN("Surface width {} capped to minimum {}",
                 _size.width,
                 caps.minImageExtent.width);

        _size.width = caps.minImageExtent.width;
    }
    else if(_size.width > caps.maxImageExtent.width) {
        BTX_WARN("Surface width {} capped to maximum {}",
                 _size.width,
                 caps.minImageExtent.width);

        _size.width = caps.maxImageExtent.width;
    }

    if(_size.height < caps.minImageExtent.height) {
        BTX_WARN("Surface height {} capped to minimum {}",
                 _size.height,
                 caps.minImageExtent.height);

        _size.height = caps.minImageExtent.height;
    }
    else if(_size.height > caps.maxImageExtent.height) {
        BTX_WARN("Surface height {} capped to maximum {}",
                 _size.height,
                 caps.minImageExtent.height);

        _size.height = caps.maxImageExtent.height;
    }

    // Update the aspect ratio
    _aspect_ratio = static_cast<float>(_size.width) /
                    static_cast<float>(_size.height);

    // Reserve minimum image count plus one so the CPU always has something to
    // work on while the GPU does its thing
    auto const image_count = caps.minImageCount + 1;
    if(_images.size() != image_count && _image_views.size() != image_count) {
        _images.clear();
        _image_views.clear();

        _images.reserve(image_count);
        _image_views.reserve(image_count);

        // Fill in the still undefined images with in-place construction
        std::generate_n(
            std::back_inserter(_images),
            _images.capacity(),
            []() {
                return std::make_unique<vkImage>();
            }
        );

        // And likewise the views
        std::generate_n(
            std::back_inserter(_image_views),
            _image_views.capacity(),
            []() {
                return std::make_unique<vkImageView>();
            }
        );
    }
}

// =============================================================================
void vkSwapchain::_query_surface_format(vk::SurfaceKHR const &surface) {
    auto const formats =
        vkPhysicalDevice::native().getSurfaceFormatsKHR(surface);

    BTX_TRACE("Found {} surface formats.", formats.size());

    // These format details were chosen to produce the most intuitive and/or
    // predictable results on the average desktop disaply
    auto const desired_format = vk::Format::eB8G8R8A8Unorm;
    auto const deisred_space = vk::ColorSpaceKHR::eSrgbNonlinear;

    bool found_desired = false;

    for(auto const& format : formats) {
        if(format.format == desired_format &&
           format.colorSpace == deisred_space)
        {
            _image_format = format;
            found_desired = true;
        }

        BTX_TRACE(
            "    {} / {}",
            vk::to_string(format.format),
            vk::to_string(format.colorSpace)
        );
    }

    if(!found_desired) {
        // In the event that our desired combination isn't found, just go with
        // whatever the implementation has as its first result
        _image_format = formats[0];

        BTX_WARN(
            "Could not find desired swapchain surface format/color space of "
            "{} / {}. Defaulting instead to {} / {}.",
            vk::to_string(desired_format),
            vk::to_string(deisred_space),
            vk::to_string(_image_format.format),
            vk::to_string(_image_format.colorSpace)
        );
    }
}

// =============================================================================
void vkSwapchain::_query_surface_present_modes(vk::SurfaceKHR const &surface) {
    auto const modes =
        vkPhysicalDevice::native().getSurfacePresentModesKHR(surface);

    BTX_TRACE("Found {} present modes.", modes.size());

    // This is the order of preference for present modes:
    bool has_fifo_relaxed = false;  // V-Sync on, but with some latency tweaks
    bool has_fifo         = false;  // Strict V-Sync
    bool has_immediate    = false;  // V-Sync off; draw as fast as possible

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

    // Use a FIFO variant if they're available and V-Sync has been chosen by
    // the user
    if(has_fifo_relaxed && RenderConfig::vsync_on) {
        _present_mode = vk::PresentModeKHR::eFifoRelaxed;
    }
    else if(has_fifo && RenderConfig::vsync_on) {
        _present_mode = vk::PresentModeKHR::eFifo;
    }
    else if(RenderConfig::vsync_on) {
        BTX_WARN("V-Sync requested but the available present modes don't "
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
vk::SwapchainCreateInfoKHR
vkSwapchain::_populate_create_info(vk::SurfaceKHR const &surface)
{
    BTX_TRACE(
        "\nSwapchain Create Info:"
        "\n    Extent:       {}x{}"
        "\n    Offset:       {}x{}"
        "\n    Aspect Ratio: {:.04f}"
        "\n    Image Count:  {}"
        "\n    Format:       {}"
        "\n    Color Space:  {}"
        "\n    Present Mode: {}",
        _size.width, _size.height,
        _offset.x, _offset.y,
        _aspect_ratio,
        _images.size(),
        vk::to_string(_image_format.format),
        vk::to_string(_image_format.colorSpace),
        vk::to_string(_present_mode)
    );

    return {
        .surface         = surface,
        .minImageCount   = static_cast<uint32_t>(_images.size()),
        .imageFormat     = _image_format.format,
        .imageColorSpace = _image_format.colorSpace,
        .imageExtent     = { .width = _size.width, .height = _size.height },

        // Image array layers will always be one, except in the case of a
        // device with multiple displays interested in the same swapchain, like
        // a VR headset
        .imageArrayLayers = 1u,

        // Marking the images in this swapchain as color attachments means they
        // can be used to draw into
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,

        // We don't need to worry about sharing images between queues because
        // vkPhysicalDevice insisted on having a queue family that could accept
        // both graphics and present commands
        .imageSharingMode = vk::SharingMode::eExclusive,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,

        // Transforms are what you'd expect: the image can be flipped, rotated,
        // etc. Leaving it alone gets us what we want for now.
        .preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity,

        // No support for transparency or color blending for now
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,

        // Store the selected present mode
        .presentMode = _present_mode,

        // Setting clipped to true allows Vulkan to ignore pixels on the render
        // surface that can't be seen. For example, moving another window in
        // front of the render surface
        .clipped = 1u,

        // There are situations during which you'll want to recreate the
        // swapchain, and providing an existing example can speed up the
        // process.
        .oldSwapchain = nullptr,
    };
}

// =============================================================================
void vkSwapchain::_get_swapchain_images() {
    auto const swapchain_images =
        _device.getSwapchainImagesKHR(_handle);

    if(swapchain_images.size() != _images.size()) {
        BTX_CRITICAL("Swapchain provided {} images, expected {}",
                     swapchain_images.size(), _images.size());
        return;
    }

    BTX_TRACE("Acquired {} swapchain images", swapchain_images.size());

    for(uint32_t i = 0u; i < _images.size(); ++i) {
        if(_images[i]->native() != nullptr) {
            _images[i]->destroy();
        }

        _images[i]->create(
            swapchain_images[i],
            _image_format.format
        );

        if(_image_views[i]->native() != nullptr) {
            _image_views[i]->destroy();
        }

        _image_views[i]->create(
            *_images[i],
            vk::ImageViewType::e2D,
            vk::ImageAspectFlagBits::eColor
        );
    }
}

} // namespace btx