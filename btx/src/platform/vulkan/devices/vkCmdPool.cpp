#include "brasstacks/platform/vulkan/devices/vkCmdPool.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdQueue.hpp"

namespace btx {

// =============================================================================
void vkCmdPool::reset(vk::CommandPoolResetFlags const flags) const {
    _device.native().resetCommandPool(_handle, flags);
}

// =============================================================================
vkCmdPool::vkCmdPool(vkDevice const &device,
                     vk::CommandPoolCreateFlags const flags) :
    _device { device },
    _handle { }
{
    vk::CommandPoolCreateInfo const pool_info {
        .flags = flags,
        .queueFamilyIndex = _device.cmd_queue().index()
    };

    auto const result = _device.native().createCommandPool(
        &pool_info,
        nullptr,
        &_handle
    );

    if(result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to create command pool: '{}'",
                     vk::to_string(result));
        return;
    }

    BTX_TRACE("Created command pool {:#x}.",
                reinterpret_cast<uint64_t>(::VkCommandPool(_handle)));
}

vkCmdPool::~vkCmdPool() {
    BTX_TRACE("Destroying command pool {:#x}.",
                reinterpret_cast<uint64_t>(::VkCommandPool(_handle)));

    _device.native().destroyCommandPool(_handle);
    _handle = nullptr;
}

} // namespace btx