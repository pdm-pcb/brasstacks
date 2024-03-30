#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/Renderer.hpp"

#include "brasstacks/core/TargetWindow.hpp"
#include "brasstacks/platform/vulkan/vkInstance.hpp"
#include "brasstacks/platform/vulkan/vkAllocator.hpp"
#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"
#include "brasstacks/platform/vulkan/devices/vkQueue.hpp"
#include "brasstacks/events/EventBus.hpp"

#include "brasstacks/platform/vulkan/descriptors/vkDescriptorPool.hpp"

namespace btx {

Application *Renderer::_application { nullptr };

auto Renderer::_surface   { std::make_unique<vkSurface>() };
auto Renderer::_device    { std::make_unique<vkDevice>() };
auto Renderer::_swapchain { std::make_unique<vkSwapchain>() };

std::vector<std::unique_ptr<vkFrameSync>> Renderer::_frame_sync;
uint32_t Renderer::_image_index { std::numeric_limits<uint32_t>::max() };

auto Renderer::_descriptor_pool { std::make_unique<vkDescriptorPool>() };

// =============================================================================
void Renderer::init(Application *application) {
    vkInstance::create(VK_TARGET_VERSION);

    _application = application;
    _create_surface();
    _select_physical_device();
    _create_device();
    _create_allocator(VK_TARGET_VERSION);
    _create_swapchain();
    _create_frame_sync();

    _descriptor_pool->create(
        vk::DescriptorPoolCreateFlags { },
        1000u,
        {
            { vk::DescriptorType::eUniformBuffer,        1000u, },
            { vk::DescriptorType::eCombinedImageSampler, 1000u, },
        }
    );

    CameraController::init();
}

// =============================================================================
void Renderer::shutdown() {
    CameraController::shutdown();
    _descriptor_pool->destroy();
    _destroy_frame_sync();
    _destroy_swapchain();
    _device->destroy();
    vkAllocator::destroy();
    _surface->destroy();
    vkInstance::destroy();
}

// =============================================================================
void Renderer::run() {
    auto const swapchain_index = _acquire_next_image();
    if(swapchain_index == std::numeric_limits<uint32_t>::max()) {
        BTX_WARN("Swapchain provided invalid index.");
        recreate_swapchain();
        return;
    }

    if(swapchain_index != _image_index) {
        BTX_ERROR("Swapchain reported {} and we're on {}",
                  swapchain_index, _image_index);
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
uint32_t Renderer::_acquire_next_image() {
    _image_index = (_image_index + 1u) % _swapchain->images().size();
    auto &frame = *_frame_sync[_image_index];
    frame.wait_and_reset();

    return _swapchain->get_next_image_index(frame.present_semaphore());
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
    if(_surface->native() != nullptr) {
        BTX_CRITICAL("Surface already created");
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

    _surface->create(create_info);
}

// =============================================================================
void Renderer::_select_physical_device() {
    if(_surface->native() == nullptr) {
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
    if(_device->native() != nullptr) {
        BTX_ERROR("Logical device already created.");
        return;
    }

    _device->create({
#ifdef BTX_DEBUG
        "VK_LAYER_KHRONOS_validation",
#endif // BTX_DEBUG
    });
}

// =============================================================================
void Renderer::_create_allocator(uint32_t const api_version) {
    vkAllocator::create(api_version);
}

// =============================================================================
void Renderer::_create_swapchain() {
    if(_swapchain->native() != nullptr) {
        BTX_CRITICAL("Swapchain already created.");
        return;
    }

    _swapchain->create(*_surface);
    _image_index = std::numeric_limits<uint32_t>::max();
}

// =============================================================================
void Renderer::_create_frame_sync() {
    auto const image_count = _swapchain->images().size();
    if(_frame_sync.size() != image_count) {
        _frame_sync.clear();
        _frame_sync.reserve(image_count);

        // Fill in the frame sync structs with default in-place construction
        std::generate_n(
            std::back_inserter(_frame_sync),
            _frame_sync.capacity(),
            []() {
                return std::make_unique<vkFrameSync>();
            }
        );
    }

    for(auto &sync_struct : _frame_sync) {
        sync_struct->create_sync_primitives();
        sync_struct->create_cmd_structures();
    }
}

// =============================================================================
void Renderer::_destroy_swapchain() {
    _swapchain->destroy();
}

// =============================================================================
void Renderer::_destroy_frame_sync() {
    for(auto &sync_struct : _frame_sync) {
        sync_struct->destroy_sync_primitives();
        sync_struct->destroy_cmd_structures();
    }
}

} // namespace btx