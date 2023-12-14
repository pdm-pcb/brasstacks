#include "brasstacks/platform/vulkan/resources/vkImage.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"

namespace btx {

// =============================================================================
vkImage::vkImage(vkDevice  const &device, vk::Image const &handle,
                 vk::Format const format) :
    _device       { device },
    _handle       { handle },
    _format       { format },
    _view         { nullptr }
{ }

vkImage::~vkImage() {
    BTX_TRACE("Destroying image {:#x}",
            reinterpret_cast<uint64_t>(::VkImage(_handle)));
    _device.native().destroyImage(_handle);
}

} // namespace btx