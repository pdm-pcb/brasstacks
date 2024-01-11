#include "Demo.hpp"

#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"
#include "brasstacks/platform/vulkan/rendering/vkSwapchain.hpp"
#include "brasstacks/platform/vulkan/rendering/vkColorPass.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkPipeline.hpp"
#include "brasstacks/platform/vulkan/rendering/vkFramebuffer.hpp"
#include "brasstacks/platform/vulkan/rendering/vkColorPass.hpp"
#include "brasstacks/platform/vulkan/resources/vkImageView.hpp"
#include "brasstacks/config/RenderConfig.hpp"

#include "brasstacks/rendering/meshes/PlaneMesh.hpp"

Demo::Demo() :
    Application("Demo"),
    _device { nullptr },
    _swapchain { nullptr },
    _color_pass { nullptr },
    _color_pipeline { nullptr },
    _color_framebuffers { }
{ }

void Demo::init(btx::vkDevice const &device, btx::vkSwapchain const &swapchain)
{
    _device = &device;
    _swapchain = &swapchain;
    _create_color_pass();
}

void Demo::shutdown() {
    _destroy_color_pass();
}

void Demo::update() {

}

void Demo::record_commands(btx::vkCmdBuffer const &cmd_buffer,
                           uint32_t const image_index)
{
    auto const &framebuffer = *_color_framebuffers[image_index];

    static std::array<vk::ClearValue, 2> const clear_values = {{
        { .color = std::array<float, 4> {{ 0.08f, 0.08f, 0.16f, 1.0f }} },
        { .depthStencil { .depth = 1.0f, .stencil = 1u } }
    }};

    vk::Rect2D const render_area = {
        .offset { .x = 0u, .y = 0u },
        .extent {
            .width  = btx::RenderConfig::swapchain_image_size.width,
            .height = btx::RenderConfig::swapchain_image_size.height,
        },
    };

    cmd_buffer.begin_render_pass(
        vk::RenderPassBeginInfo {
            .pNext           = nullptr,
            .renderPass      = _color_pass->native(),
            .framebuffer     = framebuffer.native(),
            .renderArea      = render_area,
            .clearValueCount = static_cast<uint32_t>(clear_values.size()),
            .pClearValues    = clear_values.data(),
        }
    );

    _color_pipeline->bind(cmd_buffer);

    // send push constants
    // bind descriptor sets
    // draw indexed
    // etc

    cmd_buffer.end_render_pass();
}

void Demo::_create_color_pass() {
    _color_pass =
        new btx::vkColorPass(
            *_device,
            _swapchain->image_format(),
            {
                .width  = btx::RenderConfig::swapchain_image_size.width,
                .height = btx::RenderConfig::swapchain_image_size.height,
            },
            true
        );

    _color_pipeline = new btx::vkPipeline(*_device);
    (*_color_pipeline)
        .module_from_spirv("shaders/demo.vert",
                           vk::ShaderStageFlagBits::eVertex)
        .module_from_spirv("shaders/demo.frag",
                           vk::ShaderStageFlagBits::eFragment)
        .describe_vertex_input(btx::Vertex::bindings, btx::Vertex::attributes)
        .create(
            *_color_pass,
            {
                .color_formats = { _swapchain->image_format() },
                .depth_format = vk::Format::eUndefined,
                .viewport_extent = {
                    .width  = btx::RenderConfig::swapchain_image_size.width,
                    .height = btx::RenderConfig::swapchain_image_size.height,
                },
                .viewport_offset = { .x = 0u, .y = 0u, },
                .sample_flags = _color_pass->msaa_samples(),
                .enable_depth_test = VK_TRUE,
            }
        );

    auto const swapchain_image_count = _swapchain->image_views().size();
    for(size_t i = 0; i < swapchain_image_count; ++i) {
        _color_framebuffers.emplace_back(new btx::vkFramebuffer(
            *_device, *_color_pass,
            {
                .width  = btx::RenderConfig::swapchain_image_size.width,
                .height = btx::RenderConfig::swapchain_image_size.height,
            },
            {{
                _color_pass->color_views()[i]->native(),
                _color_pass->depth_views()[i]->native(),
                _swapchain->image_views()[i]->native()
            }}
        ));
    }
}

void Demo::_destroy_color_pass() {
    for(auto *framebuffer : _color_framebuffers) {
        delete framebuffer;
    }

    delete _color_pipeline;
    delete _color_pass;
}