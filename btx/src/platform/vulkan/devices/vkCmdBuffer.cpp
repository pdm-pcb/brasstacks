#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdPool.hpp"

namespace btx {

// =============================================================================
void vkCmdBuffer::begin_one_time_submit() const {
    vk::CommandBufferBeginInfo const begin_info {
        .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
    };

    auto const result = _handle.begin(begin_info);
    if(result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to begin recording to one-shot command buffer.");
    }
}

// =============================================================================
void vkCmdBuffer::end_recording() const {
    auto const result = _handle.end();
    if(result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to end command buffer recording.");
    }
}

// =============================================================================
void vkCmdBuffer::begin_render_pass(vk::RenderPassBeginInfo const &info) const {
    _handle.beginRenderPass(info, vk::SubpassContents::eInline);
}

// =============================================================================
void vkCmdBuffer::end_render_pass() const {
    _handle.endRenderPass();
}

// =============================================================================
void vkCmdBuffer::submit_and_wait_on_device() const {
    vk::SubmitInfo const submit_info {
        .pNext                = nullptr,
        .waitSemaphoreCount   = 0u,
        .pWaitSemaphores      = nullptr,
        .pWaitDstStageMask    = nullptr,
        .commandBufferCount   = 1u,
        .pCommandBuffers      = &_handle,
        .signalSemaphoreCount = 0u,
        .pSignalSemaphores    = nullptr,
    };

    _device.submit(submit_info);
    _device.wait_idle();
}
// =============================================================================
vkCmdBuffer::vkCmdBuffer(vkDevice const &device, vkCmdPool const &pool) :
    _device { device },
    _pool   { pool },
    _handle { nullptr }
{
    vk::CommandBufferAllocateInfo const buffer_info {
        .pNext = nullptr,
        .commandPool = _pool.native(),
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1u,
    };

    auto const result = _device.native().allocateCommandBuffers(
        &buffer_info,
        &_handle
    );

    if(result != vk::Result::eSuccess) {
        BTX_CRITICAL(
            "Failed to llocated command buffer from pool {:#x}: '{}'",
            reinterpret_cast<uint64_t>(VkCommandPool(buffer_info.commandPool)),
            vk::to_string(result)
        );

        return;
    }

    BTX_TRACE(
        "Allocated command buffer {:#x} from pool {:#x}",
        reinterpret_cast<uint64_t>(VkCommandBuffer(_handle)),
        reinterpret_cast<uint64_t>(VkCommandPool(buffer_info.commandPool))
    );
}

vkCmdBuffer::~vkCmdBuffer() {
    if(_handle) {
        BTX_TRACE("Freeing command buffer {:#x}",
                  reinterpret_cast<uint64_t>(VkCommandBuffer(_handle)));

        _device.native().freeCommandBuffers(_pool.native(), { _handle });
        _handle = nullptr;
    }
}

} // namespace btx