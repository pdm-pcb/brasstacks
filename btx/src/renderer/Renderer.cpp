#include "brasstacks/renderer/Renderer.hpp"

#include "brasstacks/platform/vulkan/vkInstance.hpp"
#include "brasstacks/platform/vulkan/vkSurface.hpp"
#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdPool.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/vkSwapchain.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkPipeline.hpp"
#include "brasstacks/platform/vulkan/rendering/vkFrame.hpp"

#include "brasstacks/system/TargetWindow.hpp"

namespace btx {

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
        }
    );

    _device = new vkDevice(
        *_adapter
#ifdef BTX_DEBUG
        , { "VK_LAYER_KHRONOS_validation", }
#endif // BTX_DEBUG
    );

    _cmd_pool = new vkCmdPool(*_device);

    _swapchain = new vkSwapchain(*_adapter, *_surface, *_device);

    _frames.resize(_swapchain->image_count());
    for(auto &frame : _frames) {
        frame = new vkFrame(*_device);
    }

    // _pipeline = new vkPipeline(*_device);
    // (*_pipeline)
    //     .vert_from_spirv("shaders/01flat_color.vert")
    //     .frag_from_spirv("shaders/01flat_color.frag")
    //     .describe_vertex_input(
    //         {{
    //             .binding   = 0u,
    //             .stride    = sizeof(float) * 4u * 2u,
    //             .inputRate = vk::VertexInputRate::eVertex
    //         }},
    //         {{
    //             .location = 0u,
    //             .binding  = 0u,
    //             .format   = vk::Format::eR32G32B32A32Sfloat,
    //             .offset   = 0u,
    //         },
    //         {
    //             .location = 1u,
    //             .binding  = 0u,
    //             .format   = vk::Format::eR32G32B32A32Sfloat,
    //             .offset   = sizeof(float) * 4u,
    //         }}
    //     )
    //     .add_descriptor_set({ })
    //     .create({
    //         .color_formats = {
    //             _swapchain->image_format()
    //         },
    //         .depth_format    = _adapter->depth_format(),
    //         .viewport_extent = _swapchain->render_area().extent,
    //         .viewport_offset = _swapchain->render_area().offset,
    //         .sample_flags    = _adapter->max_msaa_flag(),
    //     });
}

Renderer::~Renderer() {
    // delete _pipeline;

    for(auto &frame : _frames) {
        delete frame;
    }

    delete _swapchain;
    delete _cmd_pool;
    delete _device;
    delete _surface;
    delete _instance;
}

} // namespace btx