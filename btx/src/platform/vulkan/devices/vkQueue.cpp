#include "brasstacks/core.hpp"
#include "brasstacks/platform/vulkan/vulkan_formatters.hpp"
#include "brasstacks/platform/vulkan/devices/vkQueue.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"

namespace btx {

// =============================================================================
vkQueue::vkQueue(vkDevice const &device, uint32_t const queue_family_index) :
    _index { queue_family_index },
    _device { device }
{
    _handle = _device.native().getQueue(queue_family_index, 0u);

    if(!_handle) {
        BTX_CRITICAL("Failed to retrieve device queue.");
        return;
    }

    BTX_TRACE("Received queue {} from device.", _handle);
}

} // namespace btx