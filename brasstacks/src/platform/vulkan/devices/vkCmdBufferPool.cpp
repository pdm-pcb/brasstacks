#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBufferPool.hpp"

#include "brasstacks/platform/vulkan/devices/vkQueue.hpp"

namespace btx {

// =============================================================================
vkCmdBufferPool::vkCmdBufferPool() :
    _handle { nullptr },
    _device { nullptr }
{ }

// =============================================================================
vkCmdBufferPool::~vkCmdBufferPool() {
    if(_handle != nullptr) {
        destroy();
    }
}

// =============================================================================
void vkCmdBufferPool::create(uint32_t const queue_index,
                             vk::CommandPoolCreateFlags const flags) {
    if(_handle != nullptr) {
        BTX_CRITICAL("vkCmdBufferPool {} already exists", _handle);
        return;
    }

    _device = Renderer::device().native();

    vk::CommandPoolCreateInfo const pool_info {
        .flags = flags,
        .queueFamilyIndex = queue_index,
    };

    auto const result = _device.createCommandPool(
        &pool_info,
        nullptr,
        &_handle
    );

    if(result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to create command pool: '{}'",
                     vk::to_string(result));
        return;
    }

    BTX_TRACE("Created command pool {}", _handle);
}

// =============================================================================
void vkCmdBufferPool::destroy() {
    BTX_TRACE("Destroying command pool {}", _handle);
    _device.destroyCommandPool(_handle);
    _handle = nullptr;
}

// =============================================================================
void vkCmdBufferPool::reset(vk::CommandPoolResetFlags const flags) const {
    _device.resetCommandPool(_handle, flags);
}

} // namespace btx