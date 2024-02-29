#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/Renderer.hpp"

#include "brasstacks/core/TargetWindow.hpp"
#include "brasstacks/platform/vulkan/vkInstance.hpp"
#include "brasstacks/platform/vulkan/rendering/vkSurface.hpp"
#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/rendering/vkSwapchain.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"
#include "brasstacks/platform/vulkan/devices/vkQueue.hpp"

namespace btx {

// =============================================================================
Renderer::Renderer(Application const &application) :
    _application        { application },
    _surface            { nullptr },
    _device             { nullptr },
    _swapchain          { nullptr },
    _image_acquire_sems { },
    _frame_sync         { },
    _image_index        { std::numeric_limits<uint32_t>::max() },
    _thread_running     { },
    _loop_running       { }
{
    vkInstance::create();

    _create_surface();
    _select_physical_device();
    _create_device();
    _create_swapchain();
    _create_frame_sync();

    _thread_running.clear();
    _loop_running.clear();
}

// =============================================================================
Renderer::~Renderer() {
    _destroy_frame_sync();
    _destroy_swapchain();

    delete _device;
    delete _surface;
}

// =============================================================================
void Renderer::start_thread() {
    BTX_TRACE("Starting renderer thread...");

    // If we just set _thread_running but not _loop_running, the renderer's
    // main loop will just pause right away
    _loop_running.test_and_set();
    _loop_running.notify_one();

    _thread_running.test_and_set();
    _thread_running.notify_one();
}

// =============================================================================
void Renderer::stop_thread() {
    BTX_TRACE("Stopping renderer thread...");

    // First, make sure the loop can proceed
    _loop_running.test_and_set();
    _loop_running.notify_one();

    // Next, allow the loop to exit
    _thread_running.clear();
}

// =============================================================================
void Renderer::toggle_loop() {
    BTX_TRACE("Toggling renderer loop...");
    if(!_loop_running.test_and_set()) {
        _loop_running.notify_one();
    }
    else {
        _loop_running.clear();
    }
}

// =============================================================================
void Renderer::run() {
    BTX_TRACE("Renderer ready to run...");
    _thread_running.wait(false);
    BTX_TRACE("Renderer running!");

    while(_thread_running.test()) {
        if(!_loop_running.test()) {
            BTX_TRACE("Renderer loop paused...");
            _loop_running.wait(false);
            BTX_TRACE("Renderer loop playing!");
        }

        TimeKeeper::frame_start();

            uint32_t const image_index = _acquire_next_image();
            if(image_index == std::numeric_limits<uint32_t>::max()) {
                _recreate_swapchain();
                continue;
            }

            _begin_recording();

            _application.record_commands();

            _end_recording();
            _submit_commands();

            if(!_present_image()) {
                _recreate_swapchain();
            }

        TimeKeeper::frame_end();
        // BTX_TRACE("Frame time: {:.09f}", TimeKeeper::frame_time());
    }
}

// =============================================================================
void Renderer::recreate_swapchain() {
    _destroy_frame_sync();
    _destroy_swapchain();
    _create_swapchain();
    _create_frame_sync();
}

// =============================================================================
uint32_t Renderer::_acquire_next_image() {
    if(_image_acquire_sems.empty()) {
        BTX_CRITICAL("Swapchain ran out of image acquire semaphores.");
        return std::numeric_limits<uint32_t>::max();
    }

    // Grab the first available semaphore
    auto const acquire_sem = _image_acquire_sems.front();
    _image_acquire_sems.pop();

    // And ask the swapchain which index comes next
    _image_index = _swapchain->acquire_image_index(acquire_sem);

    // Something has gone wrong with the swapchain
    if(_image_index == std::numeric_limits<uint32_t>::max()) {
        _image_acquire_sems.push(acquire_sem); // Put the sem back
        wait_device_idle();                    // Wait for the device to finish
        return _image_index;                   // Now tell Application
    }

    // Wrap the corresponding frame's data for convenience
    auto &frame = *_frame_sync[_image_index];

    // Wait on this frame's queue fence, which should always be signaled by
    // the time we get here. After waiting, reset the submit fence and
    // command pool.
    frame.wait_and_reset();

    // Now swap this frame's image acquire semaphore out for the one we just
    // submitted
    if(frame.image_acquire_semaphore()) {
        _image_acquire_sems.push(frame.image_acquire_semaphore());
    }

    frame.image_acquire_semaphore() = acquire_sem;

    return _image_index;
}

// =============================================================================
void Renderer::_begin_recording() {
    auto &frame_sync  = *_frame_sync[_image_index];
    auto const &cmd_buffer  = frame_sync.cmd_buffer();
    cmd_buffer.begin_one_time_submit();
}

// =============================================================================
void Renderer::_end_recording() {
    auto &frame_sync  = *_frame_sync[_image_index];
    auto const &cmd_buffer  = frame_sync.cmd_buffer();
    cmd_buffer.end_recording();
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
        .pWaitSemaphores      = &frame_sync.image_acquire_semaphore(),
        .pWaitDstStageMask    = &wait_stage,
        .commandBufferCount   = 1u,
        .pCommandBuffers      = &frame_sync.cmd_buffer().native(),
        .signalSemaphoreCount = 1u,
        .pSignalSemaphores    = &frame_sync.cmds_complete_semaphore(),
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
    auto const present_success = _swapchain->present(frame_sync, _image_index);

    if(!present_success) {
        wait_device_idle();
    }

    return present_success;
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
        .dpy = _application.target_window().display(),
        .window = _application.target_window().native()
    };

#elif BTX_WINDOWS

    vk::Win32SurfaceCreateInfoKHR const create_info {
        .pNext = nullptr,
        .flags = { },
        .hinstance = nullptr,
        .hwnd = _application.target_window().native()
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
            vkPhysicalDevice::Features::SAMPLER_ANISOTROPY,
            vkPhysicalDevice::Features::FILL_MODE_NONSOLID,
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

    _device = new vkDevice(
#ifdef BTX_DEBUG
        { "VK_LAYER_KHRONOS_validation", }
#endif // BTX_DEBUG
    );
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

    // Create one extra semaphore because by the time the first n frames have
    // requested images from the swapchain, the queue of semaphores will be
    // empty. And since the logic in acquire_next_image() tries to get a new
    // semaphore before releasing the old one for that frame, there has to be
    // one semaphore spare.
    auto result = _device->native().createSemaphore({ });
    if(result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Unable to create semaphore: '{}'",
                        vk::to_string(result.result));
        return;
    }

    BTX_TRACE("Created image acquire semaphore {}", result.value);
    _image_acquire_sems.push(result.value);
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

    while(!_image_acquire_sems.empty()) {
        auto const sem = _image_acquire_sems.front();

        BTX_TRACE("Destroying image acquire semaphore {}", sem);
        _device->native().destroySemaphore(sem);

        _image_acquire_sems.pop();
    }
}

// =============================================================================
void Renderer::_recreate_swapchain() {
    // ...?
}

} // namespace btx