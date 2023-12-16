#include "brasstacks/renderer/Renderer.hpp"

#include "brasstacks/platform/vulkan/vkInstance.hpp"
#include "brasstacks/platform/vulkan/rendering/vkSurface.hpp"
#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdPool.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/rendering/vkSwapchain.hpp"
#include "brasstacks/platform/vulkan/rendering/vkRenderPass.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkPipeline.hpp"
#include "brasstacks/platform/vulkan/rendering/vkFrame.hpp"

#include "brasstacks/system/TargetWindow.hpp"

namespace btx {

// =============================================================================
void Renderer::acquire_next_frame() {
}

// =============================================================================
void Renderer::record_commands() {

}

// =============================================================================
void Renderer::submit_commands() {

}

// =============================================================================
void Renderer::present_image() {

}

// =============================================================================
Renderer::Renderer(TargetWindow const &target_window) :
    _instance { new vkInstance() }
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
                .viewport_extent = _swapchain->render_area().extent,
                .viewport_offset = _swapchain->render_area().offset,
                .sample_flags    = vk::SampleCountFlagBits::e1,
            }
        );
}

Renderer::~Renderer() {
    delete _pipeline;
    delete _render_pass;
    delete _swapchain;
    delete _device;
    delete _surface;
    delete _instance;
}

} // namespace btx