#include "brasstacks/renderer/Renderer.hpp"

#include "brasstacks/platform/vulkan/vkInstance.hpp"
#include "brasstacks/platform/vulkan/rendering/vkSurface.hpp"
#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"
#include "brasstacks/platform/vulkan/rendering/vkSwapchain.hpp"
#include "brasstacks/platform/vulkan/resources/vkImageView.hpp"
#include "brasstacks/platform/vulkan/rendering/vkRenderPass.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkPipeline.hpp"
#include "brasstacks/platform/vulkan/rendering/vkFrame.hpp"
#include "brasstacks/platform/vulkan/rendering/vkFramebuffer.hpp"

#include "brasstacks/system/TargetWindow.hpp"
#include "brasstacks/config/RenderConfig.hpp"

namespace btx {

// =============================================================================
void Renderer::acquire_next_frame() {
    if(_image_acquire_sems.empty()) {
        BTX_CRITICAL("Swapchain ran out of image acquire semaphores.");
        return;
    }

    // Grab the first available semaphore
    auto const acquire_sem = _image_acquire_sems.front();
    _image_acquire_sems.pop();

    // And ask the swapchain which index comes next
    _next_image_index = _swapchain->acquire_next_image_index(acquire_sem);

    // Wrap the corresponding frame's data for convenience
    auto &frame = *_frames[_next_image_index];

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
}

// =============================================================================
void Renderer::record_commands() {
    auto const &frame = *_frames[_next_image_index];
    auto const &cmd_buffer = frame.cmd_buffer();

    static vk::ClearValue const clear_value {
        .color = { RenderConfig::clear_color }
    };

    cmd_buffer.begin_one_time_submit();
    cmd_buffer.begin_render_pass(
        vk::RenderPassBeginInfo {
            .pNext           = nullptr,
            .renderPass      = _render_pass->native(),
            .framebuffer     = frame.framebuffer().native(),
            .renderArea      = _swapchain->render_area(),
            .clearValueCount = 1u,
            .pClearValues    = &clear_value,
        }
    );

    _pipeline->bind(cmd_buffer);

    cmd_buffer.native().draw(3u, 1u, 0u, 0u);

    cmd_buffer.end_render_pass();
    cmd_buffer.end_recording();
}

// =============================================================================
void Renderer::submit_commands() {
    auto &frame = *_frames[_next_image_index];

    static vk::PipelineStageFlags const wait_stage {
        vk::PipelineStageFlagBits::eColorAttachmentOutput
    };

    vk::SubmitInfo const submit_info {
        .pNext                = nullptr,
        .waitSemaphoreCount   = 1u,
        .pWaitSemaphores      = &frame.image_acquire_semaphore(),
        .pWaitDstStageMask    = &wait_stage,
        .commandBufferCount   = 1u,
        .pCommandBuffers      = &frame.cmd_buffer().native(),
        .signalSemaphoreCount = 1u,
        .pSignalSemaphores    = &frame.cmds_complete_semaphore(),
    };

    auto const result = _device->queue().native().submit(
        1u,
        &submit_info,
        frame.queue_fence()
    );

    if(result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to submit commands to device queue: '{}'",
                     vk::to_string(result));
    }
}

// =============================================================================
void Renderer::present_image() {
    _swapchain->present(*_frames[_next_image_index]);
}

// =============================================================================
Renderer::Renderer(TargetWindow const &target_window) :
    _instance { new vkInstance() },
    _image_acquire_sems { },
    _frames { },
    _next_image_index { std::numeric_limits<uint32_t>::max() }
{

#if defined(BTX_LINUX)

    vk::XlibSurfaceCreateInfoKHR const create_info {
      .pNext = nullptr,
      .flags = { },
      .dpy = nullptr,
      .window = { }
    };

#elif defined(BTX_WINDOWS)

    vk::Win32SurfaceCreateInfoKHR const create_info {
      .pNext = nullptr,
      .flags = { },
      .hinstance = nullptr,
      .hwnd = target_window.native()
    };

#endif // BTX platform

    _surface = new vkSurface(*_instance, create_info);

    _adapter = new vkPhysicalDevice(
        *_instance,
        *_surface,
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        },
        {
            vkPhysicalDevice::Features::SAMPLER_ANISOTROPY,
            vkPhysicalDevice::Features::FILL_MODE_NONSOLID,
        },
        true // Prefer dGPUs and more VRAM
    );

    _device = new vkDevice(
        *_adapter
#ifdef BTX_DEBUG
        , { "VK_LAYER_KHRONOS_validation", }
#endif // BTX_DEBUG
    );

    _swapchain = new vkSwapchain(*_adapter, *_surface, *_device);

    _render_pass = new vkRenderPass(*_device, _swapchain->image_format());

    _pipeline = new vkPipeline(*_device);
    (*_pipeline)
        .module_from_spirv("shaders/demo.vert",
                           vk::ShaderStageFlagBits::eVertex)
        .module_from_spirv("shaders/demo.frag",
                           vk::ShaderStageFlagBits::eFragment)
        .describe_vertex_input({ }, { })
        .create(
            *_render_pass,
            {
                .color_formats = {
                    _swapchain->image_format()
                },
                .depth_format    = vk::Format::eUndefined,
                .viewport_extent = _swapchain->extent(),
                .viewport_offset = _swapchain->offset(),
                .sample_flags    = vk::SampleCountFlagBits::e1,
            }
        );

    _create_frame_data();
}

Renderer::~Renderer() {
    _device->wait_idle();

    for(auto *frame : _frames) {
        delete frame;
    }

    while(!_image_acquire_sems.empty()) {
        auto const &sem = _image_acquire_sems.front();

        BTX_TRACE("Destroying image acquire semaphore {:#x}",
                  reinterpret_cast<uint64_t>(VkSemaphore(sem)));
        _device->native().destroySemaphore(sem);

        _image_acquire_sems.pop();
    }

    delete _pipeline;
    delete _render_pass;
    delete _swapchain;
    delete _device;
    delete _surface;
    delete _instance;
}

// =============================================================================
void Renderer::_create_frame_data() {
    for(auto const *image_view : _swapchain->image_views()) {
        auto const result = _device->native().createSemaphore({ });
        if(result.result != vk::Result::eSuccess) {
            BTX_CRITICAL("Unable to create semaphore: '{}'",
                         vk::to_string(result.result));
            continue;
        }

        BTX_TRACE("Created image acquire semaphore {:#x}",
                  reinterpret_cast<uint64_t>(VkSemaphore(result.value)));

        _image_acquire_sems.push(result.value);

        _frames.push_back(
            new vkFrame(
                *_device,
                *_render_pass,
                _swapchain->extent(),
                image_view->native()
            )
        );
    }

    // Create one extra semaphore because... reasons?
    auto const result = _device->native().createSemaphore({ });
    if(result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Unable to create semaphore: '{}'",
                        vk::to_string(result.result));
        return;
    }

    BTX_TRACE("Created image acquire semaphore {:#x}",
                reinterpret_cast<uint64_t>(VkSemaphore(result.value)));

    _image_acquire_sems.push(result.value);
}

} // namespace btx