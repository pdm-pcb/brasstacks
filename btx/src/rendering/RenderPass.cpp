#include "brasstacks/core.hpp"
#include "brasstacks/rendering/RenderPass.hpp"

#include "brasstacks/platform/vulkan/rendering/vkSwapchain.hpp"
#include "brasstacks/platform/vulkan/rendering/vkRenderPass.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkPipeline.hpp"
#include "brasstacks/rendering/meshes/Vertex.hpp"
#include "brasstacks/platform/vulkan/rendering/vkFramebuffer.hpp"
#include "brasstacks/config/RenderConfig.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"

namespace btx {

// =============================================================================
RenderPass::RenderPass(vkDevice const &device, vkSwapchain const &swapchain,
                       vkDescriptorSetLayout const &layout) :
    _render_extent { RenderConfig::swapchain_image_size },
    _render_offset { RenderConfig::swapchain_image_offset }
{
    _render_pass = new vkRenderPass(device, swapchain.image_format());

    _pipeline = new vkPipeline(device);
    (*_pipeline)
        .module_from_spirv("shaders/demo.vert",
                           vk::ShaderStageFlagBits::eVertex)
        .module_from_spirv("shaders/demo.frag",
                           vk::ShaderStageFlagBits::eFragment)
        .describe_vertex_input(btx::Vertex::bindings, btx::Vertex::attributes)
        .add_descriptor_set(layout)
        .create(
            *_render_pass,
            {
                .color_formats = { swapchain.image_format() },
                .depth_format    = vk::Format::eUndefined,
                .viewport_extent = {
                    .width  = _render_extent.width,
                    .height = _render_extent.height
                },
                .viewport_offset = {
                    .x = _render_offset.x,
                    .y = _render_offset.y
                },
                .sample_flags    = vk::SampleCountFlagBits::e1,
            }
        );

    for(auto const *image : swapchain.images()) {
        _framebuffers.push_back(
            new vkFramebuffer(
                device,
                *_render_pass,
                {
                    .width  = _render_extent.width,
                    .height = _render_extent.height
                },
                *image
            )
        );
    }
}

// =============================================================================
RenderPass::~RenderPass() {
    for(auto *framebuffer : _framebuffers) {
        delete framebuffer;
    }

    delete _pipeline;
    delete _render_pass;
}

// =============================================================================
void RenderPass::begin(vkCmdBuffer const &cmd_buffer,
                       uint32_t const image_index) const
{
    static vk::ClearValue const clear_value {
        .color = { btx::RenderConfig::clear_color }
    };

    auto const &framebuffer = *_framebuffers[image_index];

    cmd_buffer.begin_one_time_submit();
    cmd_buffer.begin_render_pass(
        vk::RenderPassBeginInfo {
            .pNext           = nullptr,
            .renderPass      = _render_pass->native(),
            .framebuffer     = framebuffer.native(),
            .renderArea      = _render_area,
            .clearValueCount = 1u,
            .pClearValues    = &clear_value,
        }
    );
}

// =============================================================================
void RenderPass::bind(vkCmdBuffer const &cmd_buffer,
                      vkDescriptorSet const &descriptor_set) const
{
    _pipeline->bind(cmd_buffer);
    _pipeline->bind_descriptor_set(cmd_buffer, descriptor_set);
}

} // namespace btx