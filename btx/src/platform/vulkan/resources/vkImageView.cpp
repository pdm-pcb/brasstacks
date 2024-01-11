#include "brasstacks/brasstacks.hpp"

#include "brasstacks/platform/vulkan/resources/vkImageView.hpp"

#include "brasstacks/platform/vulkan/resources/vkImage.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"

namespace btx {

// =============================================================================
vkImageView::vkImageView(vkDevice const &device, vkImage const &image,
                         vk::ImageViewType const type,
                         vk::ImageAspectFlags const aspect_flags) :
    _device { device }
{
    vk::ImageViewCreateInfo const view_info {
        .image    = image.native(),
        .viewType = type,
        .format   = image.format(),
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
        BTX_CRITICAL("Failed to create view for image {}: '{}'",
                     image.native(), vk::to_string(result.result));
        return;
    }

    _handle = result.value;
    BTX_TRACE("Created view {} for image {}", _handle, image.native());
}

// =============================================================================
vkImageView::~vkImageView() {
    BTX_TRACE("Destroying image view {}", _handle);

    _device.native().destroyImageView(_handle);
}

} // namespace btx