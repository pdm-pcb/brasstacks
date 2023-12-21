#include "brasstacks/core.hpp"
#include "brasstacks/platform/vulkan/resources//vkImage.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"

namespace btx {

// =============================================================================
vkImage::vkImage(vkDevice  const &device, vk::Image const &handle,
                 vk::Format const format) :
    _device  { device },
    _handle  { handle },
    _format  { format }
{
    _view = new vkImageView(
            _device,
            _handle,
            format,
            vk::ImageViewType::e2D,
            vk::ImageAspectFlagBits::eColor
        );
}

vkImage::~vkImage() {
    delete _view;
}

} // namespace btx