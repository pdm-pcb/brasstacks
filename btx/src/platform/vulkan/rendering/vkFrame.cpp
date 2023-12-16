#include "brasstacks/platform/vulkan/rendering/vkFrame.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdPool.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"
#include "brasstacks/platform/vulkan/rendering/vkFramebuffer.hpp"

namespace btx {

// =============================================================================
void vkFrame::wait_and_reset() const {
    auto const result = _device.native().waitForFences(
        _queue_fence,
        VK_TRUE,
        std::numeric_limits<uint64_t>::max()
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
vkFrame::vkFrame(vkDevice const &device, vkRenderPass const &render_pass,
                 vk::Extent2D const &extent, vk::ImageView const &image_view) :
    _device            { device },
    _image_acquire_sem { nullptr }
{
    _create_sync_primitives();
    _create_cmd_structures();

    _framebuffer = new vkFramebuffer(_device, render_pass, extent, image_view);
}

vkFrame::~vkFrame() {
    delete _framebuffer;

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
    _device.native().destroySemaphore(_cmds_complete_sem);

    if(_image_acquire_sem != nullptr) {
        _device.native().destroySemaphore(_image_acquire_sem);
    }
}

// =============================================================================
void vkFrame::_create_cmd_structures() {
    _cmd_pool =
        new vkCmdPool(_device, vk::CommandPoolCreateFlagBits::eTransient);

    _cmd_buffer = new vkCmdBuffer(_device, *_cmd_pool);
}

// =============================================================================
void vkFrame::_create_sync_primitives() {
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