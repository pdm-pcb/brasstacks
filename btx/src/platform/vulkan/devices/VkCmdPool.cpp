#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/devices/VkCmdPool.hpp"

#include "brasstacks/platform/vulkan/devices/VkLogicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/VkCmdQueue.hpp"

namespace btx {

// =============================================================================
void VkCmdPool::create(const vk::CommandPoolCreateFlags flags) {
    const vk::CommandPoolCreateInfo pool_info {
        .flags = flags,
        .queueFamilyIndex = VkLogicalDevice::cmd_queue().index()
    };

    auto const result = VkLogicalDevice::native().createCommandPool(
        &pool_info,
        nullptr,
        &_pool
    );

    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Failed to create command pool: '{}'",
            to_string(result)
        );
    }
    else {
        CONSOLE_TRACE(
            "Created command pool {:#x}.",
            reinterpret_cast<uint64_t>(VkCommandPool(_pool))
        );
    }
}

// =============================================================================
void VkCmdPool::destroy() {
    CONSOLE_TRACE(
        "Destroying command pool {:#x}.",
        reinterpret_cast<uint64_t>(VkCommandPool(_pool))
    );
    VkLogicalDevice::native().destroyCommandPool(_pool);
}

// =============================================================================
void VkCmdPool::reset(const vk::CommandPoolResetFlags flags) const {
    VkLogicalDevice::native().resetCommandPool(_pool, flags);
}

// =============================================================================
VkCmdPool::VkCmdPool() :
    _pool { }
{ }

VkCmdPool::VkCmdPool(VkCmdPool &&other) noexcept :
    _pool { other._pool }
{
    other._pool = nullptr;
}

} // namespace btx