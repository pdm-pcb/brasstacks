#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/resources/vkImageView.hpp"

#include "brasstacks/platform/vulkan/resources/vkImage.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"

namespace btx {

// =============================================================================
vkImageView::vkImageView() :
    _handle { nullptr },
    _device { nullptr }
{ }

// =============================================================================
vkImageView::~vkImageView() {
    if(_handle) {
        destroy();
    }
}

// =============================================================================
void vkImageView::create(vk::Image const &image,
                         vk::Format const format,
                         vk::ImageViewType const type,
                         vk::ImageAspectFlags const aspect_flags)
{
    if(_handle) {
        BTX_CRITICAL("Image view {} already exists", _handle);
        return;
    }

    _device = Renderer::device().native();

    vk::ImageViewCreateInfo const view_create_info {
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

    _handle = _device.createImageView(view_create_info);
    BTX_TRACE("Created view {} for image {}", _handle, image);
}

// =============================================================================
void vkImageView::destroy() {
    BTX_TRACE("Destroying image view {}", _handle);
    _device.destroyImageView(_handle);
    _handle = nullptr;
}

} // namespace btx