#include "brasstacks/platform/vulkan/devices/vkCmdPool.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdQueue.hpp"

namespace btx {

// =============================================================================
void vkCmdPool::create(const vk::CommandPoolCreateFlags flags) {
    const vk::CommandPoolCreateInfo pool_info {
        .flags = flags,
        .queueFamilyIndex = _device.cmd_queue().index()
    };

    auto const result = _device.native().createCommandPool(
        &pool_info,
        nullptr,
        &_handle
    );

    if(result != vk::Result::eSuccess) {
        BTX_CRITICAL(
            "Failed to create command pool: '{}'",
            vk::to_string(result)
        );
    }
    else {
        BTX_TRACE(
            "Created command pool {:#x}.",
            reinterpret_cast<uint64_t>(::VkCommandPool(_handle))
        );
    }
}

// =============================================================================
void vkCmdPool::destroy() {
    BTX_TRACE(
        "Destroying command pool {:#x}.",
        reinterpret_cast<uint64_t>(::VkCommandPool(_handle))
    );
    _device.native().destroyCommandPool(_handle);
}

// =============================================================================
void vkCmdPool::reset(const vk::CommandPoolResetFlags flags) const {
    _device.native().resetCommandPool(_handle, flags);
}

// =============================================================================
vkCmdPool::vkCmdPool(vkDevice const &device) :
    _handle { },
    _device { device }
{ }

vkCmdPool::vkCmdPool(vkCmdPool &&other) noexcept :
    _handle { other._handle },
    _device { other._device }
{
    other._handle = nullptr;
}

} // namespace btx