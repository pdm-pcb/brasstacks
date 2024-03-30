#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/Renderer.hpp"

#include "brasstacks/core/TargetWindow.hpp"
#include "brasstacks/platform/vulkan/vkInstance.hpp"
#include "brasstacks/platform/vulkan/swapchain/vkSurface.hpp"
#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"
#include "brasstacks/platform/vulkan/devices/vkQueue.hpp"
#include "brasstacks/events/EventBus.hpp"

#include "brasstacks/platform/vulkan/descriptors/vkDescriptorPool.hpp"

namespace btx {

Application *Renderer::_application { nullptr };
vkSurface   *Renderer::_surface     { nullptr };
vkDevice    *Renderer::_device      { nullptr };

vkSwapchain *Renderer::_swapchain   { nullptr };
std::vector<vkFrameSync *> Renderer::_frame_sync;
uint32_t Renderer::_image_index { std::numeric_limits<uint32_t>::max() };

vkDescriptorPool *Renderer::_descriptor_pool { nullptr };

// =============================================================================
void Renderer::init(Application *application) {
    vkInstance::create();

    _application = application;
    _create_surface();
    _select_physical_device();
    _create_device();
    _create_swapchain();
    _create_frame_sync();

    _descriptor_pool = new vkDescriptorPool(
        vk::DescriptorPoolCreateFlags { },
        1000u,
        {
            { vk::DescriptorType::eUniformBuffer,        1000u, },
            { vk::DescriptorType::eCombinedImageSampler, 1000u, },
        }
    );
}

// =============================================================================
void Renderer::shutdown() {
    delete _descriptor_pool;

    _destroy_frame_sync();
    _destroy_swapchain();

    delete _device;
    delete _surface;

    vkInstance::destroy();
}

// =============================================================================
void Renderer::run() {
    _acquire_next_image();
    if(_image_index == std::numeric_limits<uint32_t>::max()) {
        BTX_WARN("Swapchain provided invalid index.");
        recreate_swapchain();
        return;
    }

    _begin_recording();

    CameraController::update_ubo();
    _application->record_commands();

    _end_recording();
    _submit_commands();

    if(!_present_image()) {
        BTX_WARN("Swapchain presentation failed.");
        recreate_swapchain();
    }
}

// =============================================================================
void Renderer::recreate_swapchain() {
    wait_device_idle();

    _application->destroy_swapchain_resources();

    _destroy_frame_sync();
    _destroy_swapchain();
    _create_swapchain();
    _create_frame_sync();

    _application->create_swapchain_resources();

    CameraController::update_perspective();
}

// =============================================================================
void Renderer::_acquire_next_image() {
    _image_index = (_image_index + 1u) % _swapchain->images().size();
    auto &frame = *_frame_sync[_image_index];
    frame.wait_and_reset();

    auto const swapchain_says =
        _swapchain->get_next_image_index(frame.present_semaphore());

    assert(_image_index == swapchain_says);
}

// =============================================================================
void Renderer::_begin_recording() {
    _frame_sync[_image_index]->cmd_buffer().begin_one_time_submit();
}

// =============================================================================
void Renderer::_end_recording() {
    _frame_sync[_image_index]->cmd_buffer().end_recording();
}

// =============================================================================
void Renderer::_submit_commands() {
    auto &frame_sync = *_frame_sync[_image_index];

    static vk::PipelineStageFlags const wait_stage {
        vk::PipelineStageFlagBits::eColorAttachmentOutput
    };

    vk::SubmitInfo const submit_info {
        .pNext                = nullptr,
        .waitSemaphoreCount   = 1u,
        .pWaitSemaphores      = &frame_sync.present_semaphore(),
        .pWaitDstStageMask    = &wait_stage,
        .commandBufferCount   = 1u,
        .pCommandBuffers      = &frame_sync.cmd_buffer().native(),
        .signalSemaphoreCount = 1u,
        .pSignalSemaphores    = &frame_sync.queue_semaphore(),
    };

    auto const result = _device->graphics_queue().native().submit(
        1u,
        &submit_info,
        frame_sync.queue_fence()
    );

    if(result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to submit commands to device queue: '{}'",
                     vk::to_string(result));
    }
}

// =============================================================================
bool Renderer::_present_image() {
    auto &frame_sync = *_frame_sync[_image_index];
    return _swapchain->present(frame_sync, _image_index);
}

// =============================================================================
void Renderer::_create_surface() {
    if(_surface != nullptr) {
        BTX_ERROR("Surface already created");
        return;
    }

#ifdef BTX_LINUX

    vk::XlibSurfaceCreateInfoKHR const create_info {
        .pNext = nullptr,
        .flags = { },
        .dpy = TargetWindow::display(),
        .window = TargetWindow::native()
    };

#elif BTX_WINDOWS

    vk::Win32SurfaceCreateInfoKHR const create_info {
        .pNext = nullptr,
        .flags = { },
        .hinstance = nullptr,
        .hwnd = TargetWindow::native()
    };

#endif // BTX platform

    _surface = new vkSurface(create_info);
}

// =============================================================================
void Renderer::_select_physical_device() {
    if(_surface == nullptr) {
        BTX_CRITICAL("Cannot select physical device without surface.");
        return;
    }

    vkPhysicalDevice::select(
        *_surface,
        {
            vkPhysicalDevice::Features::FILL_MODE_NONSOLID,
            vkPhysicalDevice::Features::SAMPLER_ANISOTROPY,
        },
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        }
    );
}

// =============================================================================
void Renderer::_create_device() {
    if(_device != nullptr) {
        BTX_ERROR("Logical device already created.");
        return;
    }

    _device = new vkDevice(vkDevice::Layers {
#ifdef BTX_DEBUG
        "VK_LAYER_KHRONOS_validation",
#endif // BTX_DEBUG
    });
}

// =============================================================================
void Renderer::_create_swapchain() {
    if(_swapchain != nullptr) {
        BTX_ERROR("Swapchain already created.");
        return;
    }

    _swapchain = new vkSwapchain(*_device, *_surface);
}

// =============================================================================
void Renderer::_create_frame_sync() {
    auto const image_count = _swapchain->images().size();
    for(uint32_t i = 0; i < image_count; ++i) {
        _frame_sync.push_back(new vkFrameSync(*_device));
    }
}

// =============================================================================
void Renderer::_destroy_swapchain() {
    delete _swapchain;
    _swapchain = nullptr;
}

// =============================================================================
void Renderer::_destroy_frame_sync() {
    for(auto *frame : _frame_sync) {
        delete frame;
    }
    _frame_sync.clear();
}

} // namespace btx