#include "brasstacks/brasstacks.hpp"

#include "brasstacks/platform/vulkan/swapchain/vkFrameSync.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkQueue.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdPool.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"

namespace btx {

// =============================================================================
vkFrameSync::vkFrameSync(vkDevice const &device) :
    _device      { device },
    _queue_fence { nullptr },
    _present_sem { nullptr },
    _queue_sem   { nullptr },
    _cmd_pool    { nullptr },
    _cmd_buffer  { nullptr }
{
    _create_sync_primitives();
    _create_cmd_structures();
}

// =============================================================================
vkFrameSync::~vkFrameSync() {
    delete _cmd_buffer;
    delete _cmd_pool;

    BTX_TRACE("\nDestroying frame sync primitives:"
              "\n\tdevice queue fence         {}"
              "\n\tpresent complete semaphore {}"
              "\n\tqueue complete semaphore   {}",
              _queue_fence,
              _present_sem,
              _queue_sem);

    _device.native().destroyFence(_queue_fence);
    _device.native().destroySemaphore(_present_sem);
    _device.native().destroySemaphore(_queue_sem);
}

// =============================================================================
void vkFrameSync::wait_and_reset() const {
    // Wait for no more than one second
    using namespace std::chrono_literals;
    static auto const wait_period =
        std::chrono::duration_cast<std::chrono::nanoseconds>(1.0s).count();

    auto const result = _device.native().waitForFences(
        _queue_fence,   // The fence(s) to wait on
        VK_TRUE,        // Whether or not to wait on all provided fences
        wait_period     // How long to wait for these fences
    );

    if(result != vk::Result::eSuccess) {
        BTX_CRITICAL("Could not wait on queue fence: '{}",
                     vk::to_string(result));
        return;
    }

    _device.native().resetFences(_queue_fence);
    _device.native().resetCommandPool(_cmd_pool->native());
}

// =============================================================================
void vkFrameSync::_create_cmd_structures() {
    _cmd_pool = new vkCmdPool(_device, _device.graphics_queue().family_index(),
                              vk::CommandPoolCreateFlagBits::eTransient);

    _cmd_buffer = new vkCmdBuffer(_device, *_cmd_pool);
}

// =============================================================================
void vkFrameSync::_create_sync_primitives() {
    vk::FenceCreateInfo const fence_info {
        .pNext = nullptr,
        .flags = vk::FenceCreateFlagBits::eSignaled
    };

    _queue_fence = _device.native().createFence(fence_info);
    _present_sem = _device.native().createSemaphore({ });
    _queue_sem   = _device.native().createSemaphore({ });

    BTX_TRACE("\nCreated frame sync primitives:"
              "\n\tdevice queue fence         {}"
              "\n\tpresent complete semaphore {}"
              "\n\tqueue complete semaphore   {}",
              _queue_fence,
              _present_sem,
              _queue_sem);
}

} // namespace btx