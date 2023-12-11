#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/devices/VkCmdBuffer.hpp"

#include "brasstacks/platform/vulkan/devices/VkLogicalDevice.hpp"

namespace btx {

// =============================================================================
void VkCmdBuffer::begin_one_time_submit() const {
    vk::CommandBufferBeginInfo const begin_info {
        .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
    };

    auto const result = _buffer.begin(begin_info);
    if(result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to begin recording to one-shot command buffer.");
    }
}

// =============================================================================
void VkCmdBuffer::end_recording() const {
    auto const result = _buffer.end();
    if(result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to end command buffer recording.");
    }
}

// =============================================================================
void VkCmdBuffer::begin_render_pass(vk::RenderPassBeginInfo const &info) const {
    _buffer.beginRenderPass(info, vk::SubpassContents::eInline);
}

// =============================================================================
void VkCmdBuffer::end_render_pass() const {
    _buffer.endRenderPass();
}

// =============================================================================
void VkCmdBuffer::submit_and_wait_on_device() const {
    vk::SubmitInfo const submit_info {
        .commandBufferCount = 1u,
        .pCommandBuffers = &_buffer
    };
    VkLogicalDevice::submit_to_cmd_queue(submit_info);
    VkLogicalDevice::wait_idle();
}

// =============================================================================
void VkCmdBuffer::allocate(const vk::CommandPool pool, const bool primary) {
    _pool = pool;

    const vk::CommandBufferAllocateInfo buffer_info {
        .commandPool = _pool,

        // A secondary command buffer can be reused between subpasses and even
        // render passes, while a primary command buffer is tied to its pass.
        .level = (primary ? vk::CommandBufferLevel::ePrimary :
                            vk::CommandBufferLevel::eSecondary),

        .commandBufferCount = 1u,
    };

    auto const result = VkLogicalDevice::native().allocateCommandBuffers(
        &buffer_info,
        &_buffer
    );

    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Failed to llocated command buffer from pool {:#x}: '{}'",
            reinterpret_cast<uint64_t>(VkCommandPool(buffer_info.commandPool)),
            to_string(result)
        );
    }
    else {
        CONSOLE_TRACE(
            "Allocated command buffer from pool {:#x}",
            reinterpret_cast<uint64_t>(VkCommandPool(buffer_info.commandPool))
        );
    }
}

// =============================================================================
void VkCmdBuffer::free() {
    VkLogicalDevice::native().freeCommandBuffers(_pool, { _buffer });
}

// =============================================================================
VkCmdBuffer::VkCmdBuffer() :
    _pool   { },
    _buffer { }
{ }

VkCmdBuffer::VkCmdBuffer(VkCmdBuffer &&other) noexcept :
    _pool   { other._pool   },
    _buffer { other._buffer }
{
    other._pool   = nullptr;
    other._buffer = nullptr;
}

} // namespace btx