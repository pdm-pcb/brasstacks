#include "brasstacks/core.hpp"
#include "brasstacks/platform/vulkan/devices/vkQueue.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"

namespace btx {

// =============================================================================
vkQueue::vkQueue(vkDevice const &device, uint32_t const queue_family_index) :
    _index { queue_family_index },
    _device { device }
{
    BTX_TRACE("Requesting queue from device {:#x}",
              reinterpret_cast<uint64_t>(VkDevice(_device.native())));

    _handle = _device.native().getQueue(queue_family_index, 0u);

    if(!_handle) {
        BTX_CRITICAL("Failed to retrieve device queue.");
    }
}

} // namespace btx