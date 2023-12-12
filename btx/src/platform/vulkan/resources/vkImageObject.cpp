#include "brasstacks/platform/vulkan/resources/vkImageObject.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"

namespace btx {

// =============================================================================
void vkImageObject::create_view(vk::ImageViewType const type) {
    if(!_handle) {
        BTX_CRITICAL("Cannot create view for non-existant image.");
        return;
    }

    if(_format == vk::Format::eUndefined) {
        BTX_CRITICAL("Cannot create view for image with undefined format.");
        return;
    }

    vk::ImageViewCreateInfo const view_info {
        .image    = _handle,
        .viewType = type,
        .format   = _format,
        .components = {
            .r = vk::ComponentSwizzle::eR,  // If color channel values are
            .g = vk::ComponentSwizzle::eG,  // swapped for some reason, these
            .b = vk::ComponentSwizzle::eB,  // paremeters allow us to specify
            .a = vk::ComponentSwizzle::eA,  // which should go where.
        },
        .subresourceRange {
            .aspectMask = _flags, // Aspect flags describe suitable
                                  // interpretations for the image data in
                                  // memory

            .baseMipLevel   = 0u, // Starting mip level
            .levelCount     = 1u, // Total mip levels
            .baseArrayLayer = 0u, // Starting array layer
            .layerCount     = 1u, // Total array layers
        }
    };

    auto const result = _device.createImageView(view_info);
    if(result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to create view for image {:#x}",
                     reinterpret_cast<uint64_t>(::VkImage(_handle)));
        return;
    }

    _view = result.value;
    BTX_TRACE("Created view {:#x} for image {:#x}",
              reinterpret_cast<uint64_t>(::VkImageView(_view)),
              reinterpret_cast<uint64_t>(::VkImage(_handle)));
}

// =============================================================================
vkImageObject::vkImageObject(vk::Device  const &device,
                             vk::Image const &handle,
                             vk::Format const format,
                             vk::ImageLayout const layout,
                             vk::ImageAspectFlags const flags) :
    _handle { handle },
    _format { format },
    _layout { layout },
    _flags  { flags },
    _view   { nullptr },
    _device { device }
{ }

vkImageObject::~vkImageObject() {
    if(_view) {
        BTX_TRACE("Destroying view {:#x} for image {:#x}",
              reinterpret_cast<uint64_t>(::VkImageView(_view)),
              reinterpret_cast<uint64_t>(::VkImage(_handle)));
        _device.destroyImageView(_view);
        _view = nullptr;
    }
}

} // namespace btx