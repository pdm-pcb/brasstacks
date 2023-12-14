#include "brasstacks/platform/vulkan/rendering/vkFrame.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"

namespace btx {

// =============================================================================
void vkFrame::wait_on_queue_fence() const {
    vk::Fence const queue_fences[] { _queue_complete };
    auto const wait_result = _device.native().waitForFences(
        queue_fences,
        VK_TRUE,
        std::numeric_limits<uint64_t>::max()
    );

    if(wait_result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to wait on frame queue fence: '{}'",
                     vk::to_string(wait_result));
        return;
    }

    _device.native().resetFences(queue_fences);
}

// =============================================================================
void vkFrame::submit_to_device() const {
    // Once LogicalDevice has acquired an image for us, it'll signal this
    // semaphore
    vk::Semaphore const acquire_complete_sems[] {
        _acquire_complete
    };

    // We want the image to be fully acquired before beginning to write to it,
    // so if the color attachemnt output stage is reached but the above
    // semaphore  hasn't signaled, wait on it.
    vk::PipelineStageFlags const acquire_before_stage =
        vk::PipelineStageFlagBits::eColorAttachmentOutput;

    // The command buffer we're getting ready to submit
    vk::CommandBuffer const command_buffers[] {
        _cmd_buffer.native()
    };

    // When it comes time for the presentation engine to take this image back
    // and show it on the display, we'll need a semaphore for it to wait on
    // in case the command buffer hasn't been completely executed.
    vk::Semaphore const commands_complete_sems[] {
        _commands_complete
    };

    // Bring it all together in one struct
    vk::SubmitInfo const submit_info {
        .waitSemaphoreCount = static_cast<uint32_t>(
            std::size(acquire_complete_sems)
        ),
        .pWaitSemaphores   = acquire_complete_sems,
        .pWaitDstStageMask = &acquire_before_stage,
        .commandBufferCount = static_cast<uint32_t>(
            std::size(command_buffers)
        ),
        .pCommandBuffers   = command_buffers,
        .signalSemaphoreCount = static_cast<uint32_t>(
            std::size(commands_complete_sems)
        ),
        .pSignalSemaphores = commands_complete_sems,
    };

    // Submit this work to the queue
    auto const result =
        _device.cmd_queue().native().submit(submit_info, _queue_complete);

    if(result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to submit commands to device: '{}'",
                     vk::to_string(result));
    }
}

// =============================================================================
vkFrame::vkFrame(vkDevice const &device) :
    _device            { device },
    _cmd_pool          { _device },
    _cmd_buffer        { _device, _cmd_pool },
    _acquire_complete  { nullptr },
    _commands_complete { nullptr },
    _queue_complete    { nullptr },
    _image_index       { std::numeric_limits<uint32_t>::max() }
{
    _create_sync_primitives();
}

vkFrame::~vkFrame() {
    _destroy_sync_primitives();
}

vkFrame::vkFrame(vkFrame &&other) :
    _device            { std::move(other._device) },
    _cmd_pool          { std::move(other._cmd_pool) },
    _cmd_buffer        { std::move(other._cmd_buffer) },
    _acquire_complete  { std::move(other._acquire_complete) },
    _commands_complete { std::move(other._commands_complete) },
    _queue_complete    { std::move(other._queue_complete) },
    _image_index       { std::move(other._image_index) }
{
    other._acquire_complete  = nullptr;
    other._commands_complete = nullptr;
    other._queue_complete    = nullptr;
}

// =============================================================================
void vkFrame::_create_sync_primitives() {
    auto const acquire_sem_result = _device.native().createSemaphore({ });
    if(acquire_sem_result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Unable to create present semaphore: '{}'",
                        vk::to_string(acquire_sem_result.result));
        return;
    }

    _acquire_complete = acquire_sem_result.value;

    auto const cmd_sem_result = _device.native().createSemaphore({ });
    if(cmd_sem_result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Unable to swapchain draw semaphore: '{}'",
                        vk::to_string(cmd_sem_result.result));
        return;
    }

    _commands_complete = cmd_sem_result.value;

    vk::FenceCreateInfo const fence_info {
        .pNext = nullptr,
        .flags = vk::FenceCreateFlagBits::eSignaled
    };

    auto const fence_result = _device.native().createFence(fence_info);
    if(fence_result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Unable to create swapchain queue fence: '{}'",
                        vk::to_string(fence_result.result));
        return;
    }

    _queue_complete = fence_result.value;

    BTX_TRACE(
        "\nCreated frame sync primitives:"
        "\n\tacquire complete semaphore  {:#x}"
        "\n\tcommands complete semaphore {:#x}"
        "\n\tqueue complete fence        {:#x}",
        reinterpret_cast<uint64_t>(VkSemaphore(_acquire_complete)),
        reinterpret_cast<uint64_t>(VkSemaphore(_commands_complete)),
        reinterpret_cast<uint64_t>(VkFence(_queue_complete))
    );
}

// =============================================================================
void vkFrame::_destroy_sync_primitives() {
    if(_acquire_complete && _commands_complete && _queue_complete) {
        BTX_TRACE(
            "\nDestroying frame sync primitives:"
            "\n\tacquire complete semaphore  {:#x}"
            "\n\tcommands complete semaphore {:#x}"
            "\n\tqueue complete fence        {:#x}",
            reinterpret_cast<uint64_t>(VkSemaphore(_acquire_complete)),
            reinterpret_cast<uint64_t>(VkSemaphore(_commands_complete)),
            reinterpret_cast<uint64_t>(VkFence(_queue_complete))
        );

        _device.native().destroySemaphore(_acquire_complete);
        _device.native().destroySemaphore(_commands_complete);
        _device.native().destroyFence(_queue_complete);
    }
}

} // namespace btx