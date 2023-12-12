#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"

#include "brasstacks/platform/vulkan/devices/vkLogicalDevice.hpp"

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
        .commandBufferCount = 1u,
        .pCommandBuffers = &_handle
    };

    _device.submit(submit_info);
    _device.wait_idle();
}

// =============================================================================
void vkCmdBuffer::allocate(const vk::CommandPool pool, const bool primary) {
    _pool = pool;

    const vk::CommandBufferAllocateInfo buffer_info {
        .commandPool = _pool,

        // A secondary command buffer can be reused between subpasses and even
        // render passes, while a primary command buffer is tied to its pass.
        .level = (primary ? vk::CommandBufferLevel::ePrimary :
                            vk::CommandBufferLevel::eSecondary),

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
    }
    else {
        BTX_TRACE(
            "Allocated command buffer from pool {:#x}",
            reinterpret_cast<uint64_t>(VkCommandPool(buffer_info.commandPool))
        );
    }
}

// =============================================================================
void vkCmdBuffer::free() {
    _device.native().freeCommandBuffers(_pool, { _handle });
}

// =============================================================================
vkCmdBuffer::vkCmdBuffer(vkLogicalDevice const &device) :
    _pool { },
    _handle { },
    _device { device }
{ }

vkCmdBuffer::vkCmdBuffer(vkCmdBuffer &&other) noexcept :
    _pool { other._pool   },
    _handle { other._handle },
    _device { other._device }
{
    other._pool   = nullptr;
    other._handle = nullptr;
}

} // namespace btx