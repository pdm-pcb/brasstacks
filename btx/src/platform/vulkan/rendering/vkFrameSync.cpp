#include "brasstacks/core.hpp"
#include "brasstacks/platform/vulkan/rendering/vkFrameSync.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkQueue.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdPool.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"
#include "brasstacks/platform/vulkan/rendering/vkFramebuffer.hpp"

namespace btx {

// =============================================================================
vkFrameSync::vkFrameSync(vkDevice const &device) :
    _device            { device },
    _queue_fence       { nullptr },
    _image_acquire_sem { nullptr },
    _cmds_complete_sem { nullptr },
    _cmd_pool          { nullptr },
    _cmd_buffer        { nullptr }
{
    _create_sync_primitives();
    _create_cmd_structures();
}

// =============================================================================
vkFrameSync::~vkFrameSync() {
    delete _cmd_buffer;
    delete _cmd_pool;

    BTX_TRACE("\nDestroying frame sync primitives:"
              "\n\tdevice queue fence          {:#x}"
              "\n\timage acquire semaphore     {:#x}"
              "\n\tcommands complete semaphore {:#x}",
              reinterpret_cast<uint64_t>(VkFence(_queue_fence)),
              reinterpret_cast<uint64_t>(VkSemaphore(_image_acquire_sem)),
              reinterpret_cast<uint64_t>(VkSemaphore(_cmds_complete_sem)));

    _device.native().destroyFence(_queue_fence);
    _device.native().destroySemaphore(_image_acquire_sem);
    _device.native().destroySemaphore(_cmds_complete_sem);
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
    _cmd_pool = new vkCmdPool(_device, _device.graphics_queue().index(),
                              vk::CommandPoolCreateFlagBits::eTransient);

    _cmd_buffer = new vkCmdBuffer(_device, *_cmd_pool);
}

// =============================================================================
void vkFrameSync::_create_sync_primitives() {
    vk::FenceCreateInfo const fence_info {
        .pNext = nullptr,
        .flags = vk::FenceCreateFlagBits::eSignaled
    };

    auto const fence_result = _device.native().createFence(fence_info);
    if(fence_result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Unable to create fence: '{}'",
                      vk::to_string(fence_result.result));
        return;
    }

    _queue_fence = fence_result.value;

    auto img_sem_result = _device.native().createSemaphore({ });
    if(img_sem_result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Unable to create semaphore: '{}'",
                        vk::to_string(img_sem_result.result));
        return;
    }

    _image_acquire_sem = img_sem_result.value;

    auto const cmd_sem_result = _device.native().createSemaphore({ });
    if(cmd_sem_result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Unable to create semaphore: '{}'",
                     vk::to_string(cmd_sem_result.result));
        return;
    }

    _cmds_complete_sem = cmd_sem_result.value;

    BTX_TRACE("\nCreated frame sync primitives:"
              "\n\tdevice queue fence          {:#x}"
              "\n\tcommands complete semaphore {:#x}",
              reinterpret_cast<uint64_t>(VkFence(_queue_fence)),
              reinterpret_cast<uint64_t>(VkSemaphore(_cmds_complete_sem)));
}

} // namespace btx