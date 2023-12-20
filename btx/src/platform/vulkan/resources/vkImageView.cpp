#include "brasstacks/core.hpp"
#include "brasstacks/platform/vulkan/resources/images/vkImageView.hpp"

#include "brasstacks/platform/vulkan/resources/images/vkImage.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"

namespace btx {

// =============================================================================
vkImageView::vkImageView(vkDevice const &device, vk::Image const &image,
                         vk::Format const format, vk::ImageViewType const type,
                         vk::ImageAspectFlags const aspect_flags) :
    _device { device }
{
    vk::ImageViewCreateInfo const view_info {
        .image    = image,
        .viewType = type,
        .format   = format,
        .components = {
            .r = vk::ComponentSwizzle::eR,  // If color channel values are
            .g = vk::ComponentSwizzle::eG,  // swapped for some reason, these
            .b = vk::ComponentSwizzle::eB,  // paremeters allow us to specify
            .a = vk::ComponentSwizzle::eA,  // which should go where.
        },
        .subresourceRange {
            .aspectMask = aspect_flags, // Aspect flags describe suitable
                                        // interpretations for the image data
                                        // in memory

            .baseMipLevel   = 0u, // Starting mip level
            .levelCount     = 1u, // Total mip levels
            .baseArrayLayer = 0u, // Starting array layer
            .layerCount     = 1u, // Total array layers
        }
    };

    auto const result = _device.native().createImageView(view_info);
    if(result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to create view for image {:#x}: '{}'",
                     reinterpret_cast<uint64_t>(::VkImage(image)),
                     vk::to_string(result.result));
        return;
    }

    _handle = result.value;
    BTX_TRACE("Created view {:#x} for image {:#x}",
              reinterpret_cast<uint64_t>(::VkImageView(_handle)),
              reinterpret_cast<uint64_t>(::VkImage(image)));
}

// =============================================================================
vkImageView::~vkImageView() {
    BTX_TRACE("Destroying image view {:#x}",
              reinterpret_cast<uint64_t>(::VkImageView(_handle)));

    _device.native().destroyImageView(_handle);
}

} // namespace btx