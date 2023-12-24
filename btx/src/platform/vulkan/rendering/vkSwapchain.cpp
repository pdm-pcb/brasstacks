#include "brasstacks/core.hpp"
#include "brasstacks/platform/vulkan/rendering/vkSwapchain.hpp"

#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkQueue.hpp"
#include "brasstacks/platform/vulkan/rendering/vkSurface.hpp"
#include "brasstacks/system/TargetWindow.hpp"
#include "brasstacks/config/RenderConfig.hpp"
#include "brasstacks/platform/vulkan/resources/vkImage.hpp"
#include "brasstacks/platform/vulkan/rendering/vkFrameSync.hpp"

namespace btx {

// =============================================================================
vkSwapchain::vkSwapchain(vkPhysicalDevice const &physical_device,
                         vkSurface const &surface,
                         vkDevice const &device) :
    _device          { device },
    _image_format    { },
    _present_mode    { },
    _handle          { nullptr },
    _images          { }
{
    // Grab the supported image counts, resolutions, etc from the surface
    _query_surface_capabilities(physical_device.native(), surface.native());

    // This swapchain's images will have the same formatting as the provided
    // surface
    _query_surface_format(physical_device.native(), surface.native());

    // The surface also determines the presentation engine modes we can choose
    // from
    _query_surface_present_modes(physical_device.native(), surface.native());

    // Build out the create info struct
    auto const create_info = _populate_create_info(surface.native());

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

    // Now that we've got the swapchain itself, we'll need its images
    _get_swapchain_images();
}

// =============================================================================
vkSwapchain::~vkSwapchain() {
    for(auto *image : _images) {
        delete image;
    }

    BTX_TRACE("Destroying swapchain {:#x}",
              reinterpret_cast<uint64_t>(::VkSwapchainKHR(_handle)));

    _device.native().destroy(_handle);
}

// =============================================================================
uint32_t vkSwapchain::acquire_image_index(vk::Semaphore const &semaphore) {
    uint32_t next_image_index;

    // Wait for no more than one second
    using namespace std::chrono_literals;
    static auto const wait_period =
        std::chrono::duration_cast<std::chrono::nanoseconds>(1.0s).count();

    // Request the next image index and signal the provided semaphore when the
    // acquisition is complete
    auto const result = _device.native().acquireNextImageKHR(
        _handle,          // The swapchain we're trying to get an image from
        wait_period,      // How long to wait for a new image
        semaphore,        // A semaphore to signal when an image is released
        VK_NULL_HANDLE,   // A fence to signal when an image is released
        &next_image_index // The index of the next free image
    );

    if(result != vk::Result::eSuccess) {
        if(result == vk::Result::eSuboptimalKHR ||
           result == vk::Result::eErrorOutOfDateKHR)
        {
            BTX_CRITICAL("Recreate swapchain: {}", vk::to_string(result));
        }
        else {
            BTX_CRITICAL("Could not acquire next swapchain image: '{}'",
                         vk::to_string(result));
        }
    }

    // Return the index of the image that'll be safe to write next
    return next_image_index;
}

// =============================================================================
void vkSwapchain::present(vkFrameSync const &frame, uint32_t const image_index) {
    // This present call will wait on frame.cmds_complete_sem to ensure the
    // submitted batch of commands has finished
    vk::PresentInfoKHR const present_info {
        .waitSemaphoreCount = 1u,
        .pWaitSemaphores = &frame.cmds_complete_semaphore(),
        .swapchainCount  = 1u,
        .pSwapchains     = &_handle,
        .pImageIndices   = &image_index,
    };

    auto const result =
        _device.graphics_queue().native().presentKHR(present_info);

    if(result != vk::Result::eSuccess) {
        if(result == vk::Result::eSuboptimalKHR ||
           result == vk::Result::eErrorOutOfDateKHR)
        {
            BTX_CRITICAL("Recreate swapchain: {}", vk::to_string(result));
        }
        else {
            BTX_CRITICAL("Failed to submit swapchain image {}: '{}",
                         image_index, vk::to_string(result));
        }
    }
}

// =============================================================================
void vkSwapchain::_query_surface_capabilities(
    vk::PhysicalDevice const &physical_device,
    vk::SurfaceKHR const &surface)
{
    auto const result = physical_device.getSurfaceCapabilitiesKHR(surface);
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

    // We intend to draw to the whole surface
    RenderConfig::swapchain_image_size.width = capabilities.currentExtent.width;
    RenderConfig::swapchain_image_size.height = capabilities.currentExtent.height;

    // Update the aspect ratio
    RenderConfig::swapchain_aspect_ratio =
        static_cast<float>(RenderConfig::swapchain_image_size.width) /
        static_cast<float>(RenderConfig::swapchain_image_size.height);

    // TODO: this merits some actual rationale
    RenderConfig::swapchain_image_count = capabilities.minImageCount + 1;
    _images.resize(RenderConfig::swapchain_image_count);
}

// =============================================================================
void vkSwapchain::_query_surface_format(
    vk::PhysicalDevice const &physical_device,
    vk::SurfaceKHR const &surface)
{
    auto const result = physical_device.getSurfaceFormatsKHR(surface);
    if(result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Could not get surface formats: '{}'",
                      vk::to_string(result.result));
        return;
    }

    auto const &formats = result.value;
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
void vkSwapchain::_query_surface_present_modes(
    vk::PhysicalDevice const &physical_device,
    vk::SurfaceKHR const &surface)
{
    auto const result = physical_device.getSurfacePresentModesKHR(surface);
    if(result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Could not query surface present modes: '{}'",
                     vk::to_string(result.result));
        return;
    }

    auto const &modes = result.value;
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
        "\n    Image Count:  {}"
        "\n    Format:       {}"
        "\n    Color Space:  {}"
        "\n    Present Mode: {}",
        RenderConfig::swapchain_image_size.width,
        RenderConfig::swapchain_image_size.height,
        RenderConfig::swapchain_image_offset.x,
        RenderConfig::swapchain_image_offset.y,
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
        .imageExtent     = {
            .width  = RenderConfig::swapchain_image_size.width,
            .height = RenderConfig::swapchain_image_size.height
        },

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
    auto const result = _device.native().getSwapchainImagesKHR(_handle);
    if(result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Could not get swapchain images: '{}'",
                     vk::to_string(result.result));
        return;
    }

    auto const &swapchain_images = result.value;
    if(swapchain_images.size() != _images.size()) {
        BTX_CRITICAL("Swapchain provided {} images, expected {}",
                     swapchain_images.size(), _images.size());
        return;
    }

    BTX_TRACE("Acquired {} swapchain images", swapchain_images.size());

    for(uint32_t i = 0u; i < _images.size(); ++i) {
        if(_images[i] != nullptr) {
            delete _images[i];
        }

        _images[i] = new vkImage(
            _device,
            swapchain_images[i],
            _image_format.format
        );
    }
}

} // namespace btx