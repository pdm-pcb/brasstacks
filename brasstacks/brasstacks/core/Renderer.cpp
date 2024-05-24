#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/Renderer.hpp"

#include "brasstacks/events/EventBus.hpp"

#include "brasstacks/core/TargetWindow.hpp"
#include "brasstacks/platform/vulkan/swapchain/vkSurface.hpp"
#include "brasstacks/platform/vulkan/vkInstance.hpp"
#include "brasstacks/platform/vulkan/vmaAllocator.hpp"
#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"
#include "brasstacks/platform/vulkan/devices/vkQueue.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorPool.hpp"
#include "brasstacks/platform/vulkan/rendering/vkColorDepth.hpp"

#include "brasstacks/tools/cameras/CameraController.hpp"

#include "brasstacks/assets/libraries/MeshLibrary.hpp"
#include "brasstacks/assets/libraries/TextureLibrary.hpp"

#include "brasstacks/platform/imgui/UIOverlay.hpp"

namespace btx {

Application *Renderer::_application { nullptr };

vkSurface   *Renderer::_surface   { nullptr };
vkDevice     Renderer::_device    { };

vkDescriptorPool *Renderer::_descriptor_pool { nullptr };

vkSwapchain Renderer::_swapchain { };
std::vector<vkFrameSync> Renderer::_frame_sync;
uint32_t Renderer::_image_index { std::numeric_limits<uint32_t>::max() };

vkColorDepth *Renderer::_color_depth { nullptr };

// =============================================================================
void Renderer::init(Application *const application) {
    vkInstance::create(BTX_VK_TARGET_VERSION);

    _application = application;
    _create_surface();
    _populate_physical_devices();
    _create_device();
    _create_allocator(BTX_VK_TARGET_VERSION);

    _descriptor_pool = new vkDescriptorPool;
    _descriptor_pool->create(
        vk::DescriptorPoolCreateFlags { },
        1000u,
        {
            { vk::DescriptorType::eUniformBuffer,        1000u, },
            { vk::DescriptorType::eCombinedImageSampler, 1000u, },
        }
    );

    UIOverlay::create_descriptor_pool();

    _create_swapchain();
    _create_frame_sync();

    _color_depth = new vkColorDepth;
    _color_depth->create_swapchain_resources();

    MeshLibrary::init();
    TextureLibrary::init();
    CameraController::init();
}

// =============================================================================
void Renderer::shutdown() {
    CameraController::shutdown();
    MeshLibrary::shutdown();
    TextureLibrary::shutdown();

    destroy_swapchain_resources();
    _destroy_swapchain();

    delete _color_depth;
    _color_depth = nullptr;

    UIOverlay::destroy_descriptor_pool();

    _descriptor_pool->destroy();
    delete _descriptor_pool;
    _descriptor_pool = nullptr;

    vmaAllocator::destroy();

    _device.destroy();

    vkPhysicalDevice::clear_device_list();

    _surface->destroy();
    delete _surface;
    _surface = nullptr;

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
        _color_depth->begin();
        UIOverlay::record_commands();
        _application->record_commands();
        UIOverlay::render();
        _color_depth->end();
    _end_recording();
    _submit_commands();

    if(!_present_image()) {
        BTX_WARN("Swapchain presentation failed.");
        recreate_swapchain();
    }
}

// =============================================================================
void Renderer::change_device() {
    wait_device_idle();

    _application->destroy_pipeline();

    CameraController::destroy_device_resources();
    MeshLibrary::shutdown();
    TextureLibrary::shutdown();

    destroy_swapchain_resources();
    _destroy_swapchain();

    UIOverlay::destroy_descriptor_pool();
    _descriptor_pool->destroy();

    vmaAllocator::destroy();

    _device.destroy();

    // Now that everything's thoroughly undone, start anew

    vkPhysicalDevice::set_msaa_levels();
    vkPhysicalDevice::set_aniso_levels();

    _create_device();
    _create_allocator(BTX_VK_TARGET_VERSION);

    _descriptor_pool->create(
        vk::DescriptorPoolCreateFlags { },
        1000u,
        {
            { vk::DescriptorType::eUniformBuffer,        1000u, },
            { vk::DescriptorType::eCombinedImageSampler, 1000u, },
        }
    );

    UIOverlay::create_descriptor_pool();

    _create_swapchain();
    _create_frame_sync();

    // if(RenderConfig::current_msaa->msaa > 1u) {
    //     _render_pass = new vkColorDepthResolvePass;
    // }
    // else {
    //     _render_pass = new vkColorDepthPass;
    // }

    _color_depth->create_swapchain_resources();

    UIOverlay::create_swapchain_resources(_application->pipeline());

    MeshLibrary::init();
    TextureLibrary::init();
    CameraController::create_device_resources();

    _application->init();
}

// =============================================================================
void Renderer::recreate_swapchain() {
    wait_device_idle();

    destroy_swapchain_resources();
    _destroy_swapchain();
    _create_swapchain();
    create_swapchain_resources();
    _application->swapchain_updated();

    CameraController::update_perspective();
}

// =============================================================================
void Renderer::create_swapchain_resources() {
    _create_frame_sync();
    _color_depth->create_swapchain_resources();
    UIOverlay::create_swapchain_resources(_application->pipeline());
}

// =============================================================================
void Renderer::destroy_swapchain_resources() {
    UIOverlay::destroy_swapchain_resources();
    _color_depth->destroy_swapchain_resources();
    _destroy_frame_sync();
}

// =============================================================================
uint32_t Renderer::_acquire_next_image() {
    _image_index = (_image_index + 1u) % _swapchain.images().size();
    auto &frame = _frame_sync[_image_index];
    frame.wait_and_reset();

    return _swapchain.get_next_image_index(frame.present_semaphore());
}

// =============================================================================
void Renderer::_begin_recording() {
    _frame_sync[_image_index].cmd_buffer().begin_one_time_submit();
}

// =============================================================================
void Renderer::_end_recording() {
    _frame_sync[_image_index].cmd_buffer().end_recording();
}

// =============================================================================
void Renderer::_submit_commands() {
    auto &frame_sync = _frame_sync[_image_index];

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

    auto const result = _device.graphics_queue().native().submit(
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
    auto &frame_sync = _frame_sync[_image_index];
    return _swapchain.present(frame_sync, _image_index);
}

// =============================================================================
void Renderer::_create_surface() {
    if(_surface != nullptr) {
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

    _surface = new vkSurface;
    _surface->create(create_info);
}

// =============================================================================
void Renderer::_populate_physical_devices() {
    if(!_surface->native()) {
        BTX_CRITICAL("Cannot select physical device without surface.");
        return;
    }

    auto features = vk::PhysicalDeviceFeatures { };
    features.samplerAnisotropy = VK_TRUE;
    features.fillModeNonSolid = VK_TRUE;

    std::array<char const *, 3> const extensions {{
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
            VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
    }};

    vkPhysicalDevice::populate_device_list(*_surface, features, extensions);
}

// =============================================================================
void Renderer::_create_device() {
    if(_device.native()) {
        BTX_ERROR("Logical device already created.");
        return;
    }

    _device.create();
}

// =============================================================================
void Renderer::_create_allocator(uint32_t const api_version) {
    vmaAllocator::create(api_version);
}

// =============================================================================
void Renderer::_create_swapchain() {
    if(_swapchain.native()) {
        BTX_CRITICAL("Swapchain already created.");
        return;
    }

    _swapchain.create(*_surface);
    _image_index = std::numeric_limits<uint32_t>::max();
}

// =============================================================================
void Renderer::_destroy_swapchain() {
    _swapchain.destroy();
}

// =============================================================================
void Renderer::_create_frame_sync() {
    auto const image_count = _swapchain.images().size();
    if(_frame_sync.size() != image_count) {
        _destroy_frame_sync();
        _frame_sync.clear();
        _frame_sync.reserve(image_count);

        // Fill in the frame sync structs with default in-place construction
        std::generate_n(
            std::back_inserter(_frame_sync),
            _frame_sync.capacity(),
            []() {
                return vkFrameSync { };
            }
        );
    }

    for(auto &sync_struct : _frame_sync) {
        sync_struct.create_sync_primitives();
        sync_struct.create_cmd_structures();
    }
}

// =============================================================================
void Renderer::_destroy_frame_sync() {
    for(auto &sync_struct : _frame_sync) {
        sync_struct.destroy_sync_primitives();
        sync_struct.destroy_cmd_structures();
    }
}

} // namespace btx
