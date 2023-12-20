#include "brasstacks/core.hpp"
#include "brasstacks/rendering/Renderer.hpp"

#include "brasstacks/system/TargetWindow.hpp"
#include "brasstacks/config/RenderConfig.hpp"

#include "brasstacks/platform/vulkan/vkInstance.hpp"
#include "brasstacks/platform/vulkan/rendering/vkSurface.hpp"
#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkQueue.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"
#include "brasstacks/platform/vulkan/rendering/vkSwapchain.hpp"
#include "brasstacks/platform/vulkan/resources/images/vkImage.hpp"
#include "brasstacks/platform/vulkan/rendering/vkRenderPass.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkPipeline.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorPool.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorSetLayout.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorSet.hpp"
#include "brasstacks/platform/vulkan/rendering/vkFrameSync.hpp"
#include "brasstacks/platform/vulkan/rendering/vkFramebuffer.hpp"

#include "brasstacks/platform/vulkan/resources/buffers/vkBuffer.hpp"
#include "brasstacks/rendering/meshes/CubeMesh.hpp"
#include "brasstacks/tools/FPSCamera.hpp"

namespace btx {

// =============================================================================
Renderer::Renderer(TargetWindow const &target_window) :
    _instance         { new vkInstance() },
    _next_image_index { std::numeric_limits<uint32_t>::max() }
{

    // First, acquire the details necessary to construct a Vulkan surface from
    // the target window
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

    _physical_device = new vkPhysicalDevice(
        *_instance,
        *_surface,
        {
            vkPhysicalDevice::Features::SAMPLER_ANISOTROPY,
            vkPhysicalDevice::Features::FILL_MODE_NONSOLID,
        },
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        }
    );

    vkBuffer::set_memory_props(_physical_device->memory_props());

    _device = new vkDevice(
        *_physical_device
#ifdef BTX_DEBUG
        , { "VK_LAYER_KHRONOS_validation", }
#endif // BTX_DEBUG
    );

    _swapchain = new vkSwapchain(*_physical_device, *_surface, *_device);

    _render_pass = new vkRenderPass(*_device, _swapchain->image_format());

    _descriptor_pool = new vkDescriptorPool(
        *_device,
        1000u,
        {{ vk::DescriptorType::eUniformBuffer, 1000u, }}
    );

    _camera = new FPSCamera(
        *_device, *_descriptor_pool, _swapchain->images().size(),
        {
            .position = { 0.0f, 0.0f, 4.0f },
            .forward  = { 0.0f, 0.0f, 0.0f },
            .up       = { 0.0f, 1.0f, 0.0f },

            .pitch = 0.0f,
            .yaw = -90.0f,
        }
    );

    _pipeline = new vkPipeline(*_device);
    (*_pipeline)
        .module_from_spirv("shaders/demo.vert",
                           vk::ShaderStageFlagBits::eVertex)
        .module_from_spirv("shaders/demo.frag",
                           vk::ShaderStageFlagBits::eFragment)
        .describe_vertex_input(Vertex::bindings, Vertex::attributes)
        .add_descriptor_set(_camera->desc_layout())
        .create(
            *_render_pass,
            {
                .color_formats = { _swapchain->image_format() },
                .depth_format    = vk::Format::eUndefined,
                .viewport_extent = _swapchain->extent(),
                .viewport_offset = _swapchain->offset(),
                .sample_flags    = vk::SampleCountFlagBits::e1,
            }
        );

    _mesh = new CubeMesh(
        *_device,
        0.5f,
        {{
            { 1.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f },
            { 0.25f, 0.25f, 0.25f },
            { 1.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f },
            { 0.5f, 0.5f, 0.5f },
        }}
    );

    _create_frame_data();
}

// =============================================================================
Renderer::~Renderer() {
    _device->wait_idle();

    _destroy_frame_data();

    delete _camera;
    delete _mesh;

    delete _pipeline;
    delete _descriptor_pool;
    delete _render_pass;
    delete _swapchain;
    delete _device;
    delete _physical_device;
    delete _surface;
    delete _instance;
}

// =============================================================================
void Renderer::acquire_next_image() {
    if(_image_acquire_sems.empty()) {
        BTX_CRITICAL("Swapchain ran out of image acquire semaphores.");
        return;
    }

    // Grab the first available semaphore
    auto acquire_sem = _image_acquire_sems.front();
    _image_acquire_sems.pop();

    // And ask the swapchain which index comes next
    _next_image_index = _swapchain->acquire_next_image_index(acquire_sem);

    // Wrap the corresponding frame's data for convenience
    auto &frame = *_frame_sync[_next_image_index];

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
    _camera->update();

    auto const &frame_sync = *_frame_sync[_next_image_index];
    auto const &cmd_buffer = frame_sync.cmd_buffer();
    auto const &framebuffer = *_framebuffers[_next_image_index];

    static vk::ClearValue const clear_value {
        .color = { RenderConfig::clear_color }
    };

    cmd_buffer.begin_one_time_submit();
    cmd_buffer.begin_render_pass(
        vk::RenderPassBeginInfo {
            .pNext           = nullptr,
            .renderPass      = _render_pass->native(),
            .framebuffer     = framebuffer.native(),
            .renderArea      = _swapchain->render_area(),
            .clearValueCount = 1u,
            .pClearValues    = &clear_value,
        }
    );

    _camera->send_buffer_data(_next_image_index);
    _pipeline->bind(cmd_buffer);
    _pipeline->bind_descriptor_set(cmd_buffer,
                                   _camera->desc_set(_next_image_index));

    _mesh->draw_indexed(cmd_buffer);

    cmd_buffer.end_render_pass();
    cmd_buffer.end_recording();
}

// =============================================================================
void Renderer::submit_commands() {
    auto &frame = *_frame_sync[_next_image_index];

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

    auto const result = _device->graphics_queue().native().submit(
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
    _swapchain->present(*_frame_sync[_next_image_index], _next_image_index);
}

// =============================================================================
void Renderer::_create_frame_data() {
    for(auto const *image : _swapchain->images()) {
        _frame_sync.push_back(new vkFrameSync(*_device));
        _framebuffers.push_back(new vkFramebuffer(*_device,
                                                  *_render_pass,
                                                  _swapchain->extent(),
                                                  *image));
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

    BTX_TRACE("Created image acquire semaphore {:#x}",
                reinterpret_cast<uint64_t>(VkSemaphore(result.value)));

    _image_acquire_sems.push(result.value);
}

// =============================================================================
void Renderer::_destroy_frame_data() {
    for(auto *framebuffer : _framebuffers) {
        delete framebuffer;
    }

    for(auto *frame : _frame_sync) {
        delete frame;
    }

    while(!_image_acquire_sems.empty()) {
        auto const sem = _image_acquire_sems.front();

        BTX_TRACE("Destroying image acquire semaphore {:#x}",
                  reinterpret_cast<uint64_t>(VkSemaphore(sem)));
        _device->native().destroySemaphore(sem);

        _image_acquire_sems.pop();
    }
}

} // namespace btx