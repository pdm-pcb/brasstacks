#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"

#include "brasstacks/platform/vulkan/devices/vkCmdBufferPool.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkQueue.hpp"

namespace btx {

// =============================================================================
vkCmdBuffer::vkCmdBuffer() :
    _handle { nullptr },
    _pool   { nullptr }
{ }

// =============================================================================
vkCmdBuffer::vkCmdBuffer(vkCmdBuffer &&rhs) :
    _handle { rhs._handle },
    _pool   { rhs._pool }
{
    rhs._handle = nullptr;
    rhs._pool = nullptr;
}

// =============================================================================
void vkCmdBuffer::allocate(vkCmdBufferPool const &pool) {
    if(_handle) {
        BTX_CRITICAL("vkCmdBuffer already {} exists", _handle);
        return;
    }

    _pool = pool.native();

    vk::CommandBufferAllocateInfo const buffer_info {
        .pNext = nullptr,
        .commandPool = _pool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1u,
    };

    auto const result = Renderer::device().native().allocateCommandBuffers(
        &buffer_info,
        &_handle
    );

    if(result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to allocate command buffer from pool {}: '{}'",
                     buffer_info.commandPool, vk::to_string(result));
        return;
    }

    BTX_TRACE("Allocated command buffer {} from pool {}",
              _handle, buffer_info.commandPool);
}

// =============================================================================
void vkCmdBuffer::free() {
    BTX_TRACE("Freeing command buffer {}", _handle);
    Renderer::device().native().freeCommandBuffers(_pool, { _handle });
    _handle = nullptr;
}

// =============================================================================
void vkCmdBuffer::begin_one_time_submit() const {
    static vk::CommandBufferBeginInfo const begin_info {
        .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
    };

    _handle.begin(begin_info);
}

// =============================================================================
void vkCmdBuffer::end_recording() const {
    _handle.end();
}

// =============================================================================
void vkCmdBuffer::begin_rendering(vk::RenderingInfoKHR const &info) const {
    _handle.beginRenderingKHR(info);
}

// =============================================================================
void vkCmdBuffer::end_rendering() const {
    _handle.endRendering();
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

    Renderer::device().graphics_queue().native().submit(submit_info);
    Renderer::device().native().waitIdle();
}

} // namespace btx