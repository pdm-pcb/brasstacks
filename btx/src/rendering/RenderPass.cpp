#include "brasstacks/core.hpp"
#include "brasstacks/rendering/RenderPass.hpp"

#include "brasstacks/platform/vulkan/rendering/vkSwapchain.hpp"
#include "brasstacks/platform/vulkan/rendering/vkRenderPass.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkPipeline.hpp"
#include "brasstacks/rendering/meshes/Vertex.hpp"
#include "brasstacks/platform/vulkan/rendering/vkFramebuffer.hpp"

namespace btx {

// =============================================================================
RenderPass::RenderPass(vkDevice const &device, vkSwapchain const &swapchain,
                       vkDescriptorSetLayout const &layout)
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
                .viewport_extent = swapchain.extent(),
                .viewport_offset = swapchain.offset(),
                .sample_flags    = vk::SampleCountFlagBits::e1,
            }
        );

    for(auto const *image : swapchain.images()) {
        _framebuffers.push_back(
            new vkFramebuffer(device, *_render_pass, swapchain.extent(), *image)
        );
    }
}

// =============================================================================
RenderPass::~RenderPass() {
    delete _render_pass;
}

} // namespace btx