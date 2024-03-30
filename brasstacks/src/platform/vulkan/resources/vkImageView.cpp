#include "brasstacks/brasstacks.hpp"

#include "brasstacks/platform/vulkan/resources/vkImageView.hpp"

#include "brasstacks/platform/vulkan/resources/vkImage.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"

namespace btx {

// =============================================================================
vkImageView::vkImageView() :
    _handle { nullptr }
{ }

// =============================================================================
void vkImageView::create(vkImage const &image,
                         vk::ImageViewType const type,
                         vk::ImageAspectFlags const aspect_flags)
{
    vk::ImageViewCreateInfo const view_create_info {
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

    _handle = Renderer::device().native().createImageView(view_create_info);
    BTX_TRACE("Created view {} for image {}", _handle, image.native());
}

// =============================================================================
void vkImageView::destroy() {
    BTX_TRACE("Destroying image view {}", _handle);
    Renderer::device().native().destroyImageView(_handle);
    _handle = nullptr;
}

} // namespace btx