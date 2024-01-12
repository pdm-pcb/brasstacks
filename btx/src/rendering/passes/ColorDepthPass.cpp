#include "brasstacks/brasstacks.hpp"
#include "brasstacks/rendering/passes/ColorDepthPass.hpp"

#include "brasstacks/rendering/meshes/Mesh.hpp"

#include "brasstacks/platform/vulkan/rendering/vkColorDepthPass.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkPipeline.hpp"
#include "brasstacks/platform/vulkan/rendering/vkFramebuffer.hpp"
#include "brasstacks/platform/vulkan/resources/vkImageView.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"

namespace btx {

// =============================================================================
ColorDepthPass::ColorDepthPass(vkDevice const &device,
                               vkSwapchain const &swapchain) :
    _device       { device },
    _swapchain    { swapchain },
    _render_pass  { new vkColorDepthPass(_device, _swapchain, true)},
    _pipeline     { new vkPipeline(_device) },
    _framebuffers { },
    _cmd_buffer   { nullptr }
{
    _create_pipeline();
    _create_framebuffers();
}

// =============================================================================
ColorDepthPass::~ColorDepthPass() {
    for(auto *framebuffer : _framebuffers) {
        delete framebuffer;
    }
    _framebuffers.clear();

    delete _pipeline;
    _pipeline = nullptr;

    delete _render_pass;
    _render_pass = nullptr;
}

// =============================================================================
void ColorDepthPass::begin(vkCmdBuffer const &cmd_buffer,
                           uint32_t const image_index)
{
    if(_cmd_buffer != nullptr) {
        BTX_ERROR("Cannot begin another render pass");
        return;
    }

    _cmd_buffer = &cmd_buffer;

    auto const &framebuffer = *_framebuffers[image_index];

    static std::array<vk::ClearValue, 2> const clear_values = {{
        { .color { std::array<float, 4> {{ 0.08f, 0.08f, 0.16f, 1.0f }} }},
        { .depthStencil { .depth = 1.0f, .stencil = 1u } }
    }};

    vk::Rect2D const render_area = {
        .offset { .x = 0u, .y = 0u },
        .extent {
            .width  = _swapchain.size().width,
            .height = _swapchain.size().height,
        },
    };

    _cmd_buffer->begin_render_pass(
        vk::RenderPassBeginInfo {
            .pNext           = nullptr,
            .renderPass      = _render_pass->native(),
            .framebuffer     = framebuffer.native(),
            .renderArea      = render_area,
            .clearValueCount = static_cast<uint32_t>(clear_values.size()),
            .pClearValues    = clear_values.data(),
        }
    );

    _pipeline->bind(*_cmd_buffer);
}

// =============================================================================
void ColorDepthPass::end() {
    if(_cmd_buffer == nullptr) {
        BTX_ERROR("Cannot end render pass that hasn't started");
        return;
    }
    _cmd_buffer->end_render_pass();
    _cmd_buffer = nullptr;
}

// =============================================================================
void ColorDepthPass::_create_pipeline() {
    (*_pipeline)
        .module_from_spirv("shaders/demo.vert",
                           vk::ShaderStageFlagBits::eVertex)
        .module_from_spirv("shaders/demo.frag",
                           vk::ShaderStageFlagBits::eFragment)
        .describe_vertex_input(Vertex::bindings, Vertex::attributes)
        .create(
            *_render_pass,
            {
                .color_formats = { _swapchain.image_format() },
                .depth_format = vk::Format::eUndefined,
                .viewport_extent = {
                    .width  = _swapchain.size().width,
                    .height = _swapchain.size().height,
                },
                .viewport_offset = { .x = 0u, .y = 0u, },
                .sample_flags = _render_pass->msaa_samples(),
                .enable_depth_test = VK_TRUE,
            }
        );
}

// =============================================================================
void ColorDepthPass::_create_framebuffers() {
    auto const swapchain_image_count = _swapchain.image_views().size();
    for(size_t i = 0; i < swapchain_image_count; ++i) {
        _framebuffers.emplace_back(new vkFramebuffer(
            _device,
            *_render_pass,
            {
                .width  = _swapchain.size().width,
                .height = _swapchain.size().height,
            },
            {{
                _render_pass->color_views()[i]->native(),
                _render_pass->depth_views()[i]->native(),
                _swapchain.image_views()[i]->native()
            }}
        ));
    }
}

} // namepsace btx