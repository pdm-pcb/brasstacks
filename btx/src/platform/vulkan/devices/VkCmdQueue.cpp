#include "brasstacks/platform/vulkan/devices/vkCmdQueue.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"

namespace btx {

// =============================================================================
void vkCmdQueue::fill_create_info(uint32_t const index, float const priority) {
    _index    = index;
    _priority = priority;

    _create_info = vk::DeviceQueueCreateInfo {
        .queueFamilyIndex = _index,
        .queueCount       = 1u,
        .pQueuePriorities = &_priority,
    };
}

// =============================================================================
void vkCmdQueue::request_queue() {
    _handle = _device.native().getQueue(_index, 0u);
    if(!_handle) {
        BTX_CRITICAL("Could not get device queue.");
    }
}

vkCmdQueue::vkCmdQueue(vkDevice const &device) :
    _index    { std::numeric_limits<uint32_t>::max() },
    _priority { std::numeric_limits<float>::max() },
    _handle   { nullptr },
    _device   { device }
{ }

} // namespace btx