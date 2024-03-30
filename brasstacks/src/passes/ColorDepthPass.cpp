#include "brasstacks/brasstacks.hpp"
#include "brasstacks/passes/ColorDepthPass.hpp"

#include "brasstacks/platform/vulkan/swapchain/vkSwapchain.hpp"
#include "brasstacks/platform/vulkan/resources/vkImageView.hpp"

namespace btx {

// =============================================================================
ColorDepthPass::ColorDepthPass() :
    _render_pass  { },
    _pipeline     { },
    _framebuffers { },
    _cmd_buffer   { nullptr }
{ }

// =============================================================================
ColorDepthPass::~ColorDepthPass() {
    destroy_swapchain_resources();
    _pipeline.destroy();
    _render_pass.destroy();
}

// =============================================================================
void ColorDepthPass::create_pipeline() {
    _render_pass.create();

    _pipeline
        .create(
            _render_pass,
            {
                .color_formats     = { Renderer::swapchain().image_format() },
                .depth_format      = _render_pass.depth_format(),
                .viewport_extent   = Renderer::swapchain().size(),
                .viewport_offset   = Renderer::swapchain().offset(),
                .sample_flags      = _render_pass.msaa_samples(),
                .enable_depth_test = VK_TRUE,
            }
        );
}

// =============================================================================
void ColorDepthPass::destroy_swapchain_resources() {
    _destroy_framebuffers();
    _render_pass.destroy_swapchain_resources();
}

// =============================================================================
void ColorDepthPass::create_swapchain_resources() {
    _render_pass.create_swapchain_resources();

    // This is redundant when the render pass has just been created, but
    // necessary when the swapchain has changed size
    _pipeline.update_dimensions(Renderer::swapchain().size(),
                                Renderer::swapchain().offset());

    _create_framebuffers();
}

// =============================================================================
void ColorDepthPass::begin() {
    if(_cmd_buffer != nullptr) {
        BTX_ERROR("Cannot begin another render pass");
        return;
    }

    _cmd_buffer = &Renderer::cmd_buffer();

    auto const &framebuffer = *_framebuffers[Renderer::image_index()];

    static std::array<vk::ClearValue, 2> const clear_values = {{
        { .color { std::array<float, 4> {{ 0.08f, 0.08f, 0.16f, 1.0f }} }},
        { .depthStencil { .depth = 1.0f, .stencil = 1u } }
    }};

    vk::Rect2D const render_area = {
        .offset { .x = 0u, .y = 0u },
        .extent {
            .width  = Renderer::swapchain().size().width,
            .height = Renderer::swapchain().size().height,
        },
    };

    _cmd_buffer->begin_render_pass(
        vk::RenderPassBeginInfo {
            .pNext           = nullptr,
            .renderPass      = _render_pass.native(),
            .framebuffer     = framebuffer.native(),
            .renderArea      = render_area,
            .clearValueCount = static_cast<uint32_t>(clear_values.size()),
            .pClearValues    = clear_values.data(),
        }
    );

    _pipeline.bind(*_cmd_buffer);
}

// =============================================================================
void ColorDepthPass::end() {
    if(_cmd_buffer == nullptr) {
        BTX_ERROR("Cannot end render pass that hasn't started");
        return;
    }
    _cmd_buffer->end_render_pass();
    _cmd_buffer = nullptr;

    _pipeline.unbind();
}

// =============================================================================
void ColorDepthPass::bind_descriptor_set(vkDescriptorSet const &set) const {
    _pipeline.bind_descriptor_set(set);
}

// =============================================================================
void ColorDepthPass::send_push_constants(PushConstants const push_constants) {
    size_t offset = 0u;
    for(auto const& push_constant : push_constants) {
        _cmd_buffer->native().pushConstants(
            _pipeline.layout(),
            push_constant.stage_flags,
            static_cast<uint32_t>(offset),
            static_cast<uint32_t>(push_constant.size_bytes),
            push_constant.data
        );

        offset += push_constant.size_bytes;
    }
}

// =============================================================================
void ColorDepthPass::_create_framebuffers() {
    auto const image_count = Renderer::swapchain().image_views().size();
    if(_framebuffers.size() != image_count) {
        _framebuffers.clear();
        _framebuffers.reserve(image_count);

        // Fill in the still undefined images with in-place construction
        std::generate_n(
            std::back_inserter(_framebuffers),
            _framebuffers.capacity(),
            []() {
                return std::make_unique<vkFramebuffer>();
            }
        );
    }

    for(size_t i = 0; i < image_count; ++i) {
        _framebuffers[i]->create(
            _render_pass,
            Renderer::swapchain().size(),
            {{
                _render_pass.color_views()[i]->native(),
                _render_pass.depth_view().native(),
                Renderer::swapchain().image_views()[i]->native()
            }}
        );
    }
}

// =============================================================================
void ColorDepthPass::_destroy_framebuffers() {
    for(auto &framebuffer : _framebuffers) {
        framebuffer->destroy();
    }
}

} // namespace btx