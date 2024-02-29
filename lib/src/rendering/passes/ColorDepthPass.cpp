#include "brasstacks/brasstacks.hpp"
#include "brasstacks/rendering/passes/ColorDepthPass.hpp"

#include "brasstacks/rendering/meshes/Mesh.hpp"

#include "brasstacks/platform/vulkan/rendering/vkColorDepthPass.hpp"
#include "brasstacks/platform/vulkan/rendering/vkSwapchain.hpp"
#include "brasstacks/platform/vulkan/rendering/vkFramebuffer.hpp"
#include "brasstacks/platform/vulkan/resources/vkImageView.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"

namespace btx {

// =============================================================================
ColorDepthPass::ColorDepthPass(Renderer const &renderer) :
    _renderer     { renderer },
    _render_pass  { new vkColorDepthPass(_renderer, true)},
    _pipeline     { new vkPipeline(_renderer.device()) },
    _framebuffers { },
    _cmd_buffer   { nullptr }
{ }

// =============================================================================
ColorDepthPass::~ColorDepthPass() {
    destroy_swapchain_resources();

    delete _pipeline;
    _pipeline = nullptr;

    delete _render_pass;
    _render_pass = nullptr;
}

// =============================================================================
void ColorDepthPass::create() {
    _create_pipeline();
    _create_framebuffers();
}

// =============================================================================
void ColorDepthPass::destroy_swapchain_resources() {
    for(auto *framebuffer : _framebuffers) {
        delete framebuffer;
    }
    _framebuffers.clear();

    _render_pass->destroy_swapchain_resources();
}

// =============================================================================
void ColorDepthPass::recreate_swapchain_resources() {
    _render_pass->recreate_swapchain_resources();

    _pipeline->update_dimensions(_renderer.swapchain().size(),
                                 _renderer.swapchain().offset());

    _create_framebuffers();
}

// =============================================================================
void ColorDepthPass::begin() {
    if(_cmd_buffer != nullptr) {
        BTX_ERROR("Cannot begin another render pass");
        return;
    }

    _cmd_buffer = &_renderer.cmd_buffer();

    auto const &framebuffer = *_framebuffers[_renderer.image_index()];

    static std::array<vk::ClearValue, 2> const clear_values = {{
        { .color { std::array<float, 4> {{ 0.08f, 0.08f, 0.16f, 1.0f }} }},
        { .depthStencil { .depth = 1.0f, .stencil = 1u } }
    }};

    vk::Rect2D const render_area = {
        .offset { .x = 0u, .y = 0u },
        .extent {
            .width  = _renderer.swapchain().size().width,
            .height = _renderer.swapchain().size().height,
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

    _pipeline->unbind();
}

// =============================================================================
void ColorDepthPass::bind_descriptor_set(vkDescriptorSet const &set) const {
    _pipeline->bind_descriptor_set(set);
}

// =============================================================================
void ColorDepthPass::send_push_constants(
    vkPipeline::PushConstants const &push_constants)
{
    size_t offset = 0u;
    for(auto const& push_constant : push_constants) {
        _cmd_buffer->native().pushConstants(
            _pipeline->layout(),
            push_constant.stage_flags,
            static_cast<uint32_t>(offset),
            static_cast<uint32_t>(push_constant.size_bytes),
            push_constant.data
        );

        offset += push_constant.size_bytes;
    }
}

// =============================================================================
void ColorDepthPass::_create_pipeline() {
    (*_pipeline)
        .create(
            *_render_pass,
            {
                .color_formats     = { _renderer.swapchain().image_format() },
                .depth_format      = vk::Format::eUndefined,
                .viewport_extent   = _renderer.swapchain().size(),
                .viewport_offset   = _renderer.swapchain().offset(),
                .sample_flags      = _render_pass->msaa_samples(),
                .enable_depth_test = VK_TRUE,
            }
        );
}

// =============================================================================
void ColorDepthPass::_create_framebuffers() {
    auto const image_count = _renderer.swapchain().image_views().size();
    for(size_t i = 0; i < image_count; ++i) {
        _framebuffers.emplace_back(new vkFramebuffer(
            _renderer.device(),
            *_render_pass,
            _renderer.swapchain().size(),
            {{
                _render_pass->color_views()[i]->native(),
                _render_pass->depth_views()[i]->native(),
                _renderer.swapchain().image_views()[i]->native()
            }}
        ));
    }
}

} // namespace btx