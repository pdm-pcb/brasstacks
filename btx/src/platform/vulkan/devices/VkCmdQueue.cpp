#include "brasstacks/platform/vulkan/devices/VkCmdQueue.hpp"

#include "brasstacks/platform/vulkan/devices/VkLogicalDevice.hpp"

namespace btx {

// =============================================================================
void VkCmdQueue::fill_create_info(uint32_t const index, float const priority) {
    _index    = index;
    _priority = priority;

    _create_info = vk::DeviceQueueCreateInfo {
        .queueFamilyIndex = _index,
        .queueCount       = 1u,
        .pQueuePriorities = &_priority,
    };
}

// =============================================================================
void VkCmdQueue::request_queue() {
    _queue = VkLogicalDevice::native().getQueue(_index, 0u);
    if(!_queue) {
        CONSOLE_CRITICAL("Could not get device queue");
    }
}

VkCmdQueue::VkCmdQueue() :
    _index    { std::numeric_limits<uint32_t>::max() },
    _priority { std::numeric_limits<float>::max() },
    _queue    { }
{ }

} // namespace btx