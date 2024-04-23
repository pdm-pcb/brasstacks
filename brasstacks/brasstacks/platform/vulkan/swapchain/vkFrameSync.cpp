#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/swapchain/vkFrameSync.hpp"

#include "brasstacks/platform/vulkan/devices/vkQueue.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBufferPool.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"

namespace btx {

// =============================================================================
vkFrameSync::vkFrameSync() :
    _queue_fence     { nullptr },
    _present_sem     { nullptr },
    _queue_sem       { nullptr },
    _cmd_buffer_pool { new vkCmdBufferPool },
    _cmd_buffer      { new vkCmdBuffer }
{ }

// =============================================================================
vkFrameSync::~vkFrameSync() {
    delete _cmd_buffer_pool;
    delete _cmd_buffer;
}

// =============================================================================
vkFrameSync::vkFrameSync(vkFrameSync &&rhs) :
    _queue_fence     { rhs._queue_fence },
    _present_sem     { rhs._present_sem },
    _queue_sem       { rhs._queue_sem },
    _cmd_buffer_pool { rhs._cmd_buffer_pool },
    _cmd_buffer      { rhs._cmd_buffer }
{
    rhs._queue_fence     = nullptr;
    rhs._present_sem     = nullptr;
    rhs._queue_sem       = nullptr;
    rhs._cmd_buffer_pool = nullptr;
    rhs._cmd_buffer      = nullptr;
}

// =============================================================================
void vkFrameSync::wait_and_reset() const {
    // Wait for no more than one second
    using namespace std::chrono_literals;
    static auto const wait_period =
        std::chrono::duration_cast<std::chrono::nanoseconds>(1.0s).count();

    auto const result = Renderer::device().native().waitForFences(
        _queue_fence,   // The fence(s) to wait on
        VK_TRUE,        // Whether or not to wait on all provided fences
        wait_period     // How long to wait for these fences
    );

    if(result != vk::Result::eSuccess) {
        BTX_CRITICAL("Could not wait on queue fence: '{}",
                     vk::to_string(result));
        return;
    }

    Renderer::device().native().resetFences(_queue_fence);
    Renderer::device().native().resetCommandPool(_cmd_buffer_pool->native());
}

// =============================================================================
void vkFrameSync::create_cmd_structures() {
    _cmd_buffer_pool->create(Renderer::device().graphics_queue().family_index(),
                             vk::CommandPoolCreateFlagBits::eTransient);

    _cmd_buffer->allocate(*_cmd_buffer_pool);
}

// =============================================================================
void vkFrameSync::create_sync_primitives() {
    vk::FenceCreateInfo const fence_info {
        .pNext = nullptr,
        .flags = vk::FenceCreateFlagBits::eSignaled
    };

    _queue_fence = Renderer::device().native().createFence(fence_info);
    _present_sem = Renderer::device().native().createSemaphore({ });
    _queue_sem   = Renderer::device().native().createSemaphore({ });

    BTX_TRACE("\nCreated frame sync primitives:"
              "\n\tdevice queue fence         {}"
              "\n\tpresent complete semaphore {}"
              "\n\tqueue complete semaphore   {}",
              _queue_fence,
              _present_sem,
              _queue_sem);
}

// =============================================================================
void vkFrameSync::destroy_cmd_structures() {
    _cmd_buffer->free();
    _cmd_buffer_pool->destroy();
}

// =============================================================================
void vkFrameSync::destroy_sync_primitives() {
    BTX_TRACE("\nDestroying frame sync primitives:"
              "\n\tdevice queue fence         {}"
              "\n\tpresent complete semaphore {}"
              "\n\tqueue complete semaphore   {}",
              _queue_fence,
              _present_sem,
              _queue_sem);

    Renderer::device().native().destroyFence(_queue_fence);
    Renderer::device().native().destroySemaphore(_present_sem);
    Renderer::device().native().destroySemaphore(_queue_sem);
}

} // namespace btx