#include "brasstacks/platform/vulkan/pipeline/vkPipeline.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkShader.hpp"
#include "brasstacks/platform/vulkan/rendering/vkRenderPass.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"

namespace btx {

// =============================================================================
vkPipeline::vkPipeline(vkDevice const &device) :
    _device               { device },
    _shaders              { },
    _shader_stages        { },
    _viewport             { 0, 0 },
    _scissor              {{ 0, 0 }},
    _vert_input_info      { },
    _assembly_info        { },
    _viewport_info        { },
    _raster_info          { },
    _multisample_info     { },
    _depth_stencil_info   { },
    _blend_info           { },
    _dynamic_states       { },
    _dynamic_state_info   { },
    _layout               { },
    _handle               { nullptr }
{ }

// =============================================================================
vkPipeline::~vkPipeline() {
    BTX_TRACE("Destroying pipeline {:#x}, layout {:#x}",
              reinterpret_cast<uint64_t>(VkPipeline(_handle)),
              reinterpret_cast<uint64_t>(VkPipelineLayout(_layout)));

    _device.native().destroy(_layout);
    _device.native().destroy(_handle);
}

// =============================================================================
void vkPipeline::create(vkRenderPass const &render_pass, Config const &config) {
    _init_assembly();
    _init_viewport(config);
    _init_raster(config);
    _init_multisample(config);
    _init_depth_stencil(config);
    _init_blend_states();
    _init_dynamic_states();
    _init_layout();

    vk::GraphicsPipelineCreateInfo const pipeline_info {
        .pNext = nullptr,

        // If we're in a debug build, don't optimize the shaders
        #ifdef BTX_DEBUG
            .flags = vk::PipelineCreateFlagBits::eDisableOptimization,
        #endif // BTX_DEBUG

        .stageCount = static_cast<uint32_t>(_shader_stages.size()),
        .pStages    = _shader_stages.data(),

        .pVertexInputState   = &_vert_input_info,
        .pInputAssemblyState = &_assembly_info,
        .pTessellationState  = nullptr,
        .pViewportState      = &_viewport_info,
        .pRasterizationState = &_raster_info,
        .pMultisampleState   = &_multisample_info,
        .pDepthStencilState  = &_depth_stencil_info,
        .pColorBlendState    = &_blend_info,
        .pDynamicState       = &_dynamic_state_info,

        .layout              = _layout,

        .renderPass          = render_pass.native(),
        .subpass             = { },

        // A new pipeline may be derrived from an existing one, only updating
        // what needs to be updated. The .basePipeline* values designate an
        // existing pipeline to pull from.
        .basePipelineHandle  = nullptr,
        .basePipelineIndex   = 0,
    };

    auto const pipeline_result =
        _device.native().createGraphicsPipeline({ }, pipeline_info);

    if(pipeline_result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Unable to create Vulkan pipeline: '{}'",
                     vk::to_string(pipeline_result.result));
        return;
    }

    _handle = pipeline_result.value;
    BTX_TRACE("Created Vulkan pipeline {:#x}",
              reinterpret_cast<uint64_t>(VkPipeline(_handle)));

    // Destroy the shader modules now that the pipeline is baked
    for(auto *shader : _shaders) {
        delete shader;
    }
}

// =============================================================================
void vkPipeline::bind(vkCmdBuffer const &cmd_buffer) {
    cmd_buffer.native().bindPipeline(
        vk::PipelineBindPoint::eGraphics,
        _handle
    );
    cmd_buffer.native().setViewport(0u, _viewport);
    cmd_buffer.native().setScissor(0u, _scissor);
}

// =============================================================================
vkPipeline & vkPipeline::module_from_spirv(std::string_view filepath,
                                           vk::ShaderStageFlagBits const stage,
                                           std::string_view entry_point)
{
    if(_handle) {
        BTX_CRITICAL("Adding a fragment stage to a pipeline that's already "
                     "been created.");
    }

    _shaders.emplace_back(new vkShader(_device, filepath));

    _shader_stages.emplace_back(
        vk::PipelineShaderStageCreateInfo {
            .stage = stage,
            .module = _shaders.back()->native(),
            .pName = entry_point.data(),
        }
    );

    return *this;
}

// =============================================================================
vkPipeline & vkPipeline::describe_vertex_input(VertBindings const &bindings,
                                               VertAttribs const &attributes)
{
    auto const binding_count = static_cast<uint32_t>(bindings.size());
    auto const attrib_count  = static_cast<uint32_t>(attributes.size());

    _vert_input_info = {
        .vertexBindingDescriptionCount = binding_count,
        .pVertexBindingDescriptions    = bindings.data(),

        .vertexAttributeDescriptionCount = attrib_count,
        .pVertexAttributeDescriptions   = attributes.data(),
    };

    return *this;
}

// =============================================================================
void vkPipeline::update_dimensions(vk::Extent2D const &extent,
                                 vk::Offset2D const &offset)
{
    _viewport = vk::Viewport {
        .x         = static_cast<float>(offset.x),
        .y         = static_cast<float>(offset.y),
        .width     = static_cast<float>(extent.width),
        .height    = static_cast<float>(extent.height),
        .minDepth  = 0.0f,
        .maxDepth  = 1.0f,
    };

    _scissor = vk::Rect2D {
        .offset = { offset.x, offset.y },
        .extent = { extent.width, extent.height },
    };

    BTX_TRACE(
        "Pipeline viewport updated: {:.02f}x{:.02f} ({:.02f}, {:.02f}) ",
        _viewport.width,
        _viewport.height,
        _viewport.x,
        _viewport.y
    );
}

// =============================================================================
void vkPipeline::_init_assembly() {
    // The primitive assembly stage requires knowing how to interpret the
    // vertices you've asked it to draw. Again, we're not feeding anything
    // into the vertex shader, but we do want the assembly stage to see the
    // fixed vertex data as a triangle.
    _assembly_info = {
        // More correctly, the vertices we provided constitute a triangle list
        // with a length of one.
        .topology = vk::PrimitiveTopology::eTriangleList,

        // Restarting the assembly of primitives is not applicable here.
        .primitiveRestartEnable = VK_FALSE
    };
}

// =============================================================================
void vkPipeline::_init_viewport(Config const &config) {
    // We need to tell Vulkan how many viewports and scissors we're providing,
    // but since we're going to mark these as dynamic states, the actual
    // pointers should be null.
    _viewport_info = vk::PipelineViewportStateCreateInfo {
        .viewportCount = 1u,
        .pViewports    = nullptr,
        .scissorCount  = 1u,
        .pScissors     = nullptr,
    };

    update_dimensions(config.viewport_extent, config.viewport_offset);
}

// =============================================================================
void vkPipeline::_init_raster(Config const &config) {
    _raster_info = {
        // Depth clamping requires enabling a VkPhysicalDevice feature of the
        // same name, and changes how the depth test of a given fragment might
        // go by clamping its Z value to be within the near and far planes of
        // the view frustum before running the test.
        .depthClampEnable = VK_FALSE,

        // There are some situations in which you want a pipeline to complete
        // only the vertex (or geometry, etc) stage on the geometry. In those
        // situations, it's hugely beneficial to discard the primitives before
        // reaching the rasterization stage.
        .rasterizerDiscardEnable = VK_FALSE,

        .polygonMode = config.polygon_mode,
        .cullMode    = config.cull_mode,
        .frontFace   = config.front_face,

        .depthBiasEnable         = config.enable_depth_bias,
        .depthBiasConstantFactor = config.depth_bias_constant,
        .depthBiasClamp          = 0.0f,
        .depthBiasSlopeFactor    = config.depth_bias_slope,

        // If a line segment is to be reasterized, what width should it be?
        .lineWidth = 1.0f,
    };
}

// =============================================================================
void vkPipeline::_init_multisample(Config const &config) {
    _multisample_info = {
        .rasterizationSamples  = config.sample_flags,
        .sampleShadingEnable   = VK_FALSE,
        .minSampleShading      = 0.0f,
        .pSampleMask           = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable      = VK_FALSE,
    };
}

// =============================================================================
void vkPipeline::_init_depth_stencil(Config const &config) {
    _depth_stencil_info = {
        .depthTestEnable       = config.enable_depth_test,
        .depthWriteEnable      = VK_TRUE,
        .depthCompareOp        = config.depth_compare,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable     = VK_FALSE,
        .front                 = { },
        .back                  = { },
        .minDepthBounds        = 0.0f,
        .maxDepthBounds        = 1.0f
    };
}

// =============================================================================
void vkPipeline::_init_blend_states() {
    // Even though blending is disabled, the pipeline still runs a blend stage
    _blend_states = {{
        .blendEnable = VK_FALSE,

        // ...and the blend stage needs to know the color channels to which
        // it's allowed to write
        .colorWriteMask = vk::ColorComponentFlagBits::eR |
                          vk::ColorComponentFlagBits::eG |
                          vk::ColorComponentFlagBits::eB |
                          vk::ColorComponentFlagBits::eA
    }};

    _blend_info = {
        // The blend stage also needs to know what images it's blending. Since
        // our render pass writes to its color attachemnt, we need to tell
        // the blend stage to look there.
        .attachmentCount = static_cast<uint32_t>(_blend_states.size()),
        .pAttachments    = _blend_states.data()
    };
}

// =============================================================================
void vkPipeline::_init_dynamic_states() {
    // Setting the viewport and scissor states to dynamic allows us to change
    // the size of the target surface without recreating the entire pipeline.
    _dynamic_states = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };

    _dynamic_state_info = {
        .dynamicStateCount = static_cast<uint32_t>(_dynamic_states.size()),
        .pDynamicStates    = _dynamic_states.data(),
    };
}

// =============================================================================
void vkPipeline::_init_layout() {
    const vk::PipelineLayoutCreateInfo layout_info {
        .setLayoutCount         = 0u,
        .pSetLayouts            = nullptr,
        .pushConstantRangeCount = 0u,
        .pPushConstantRanges    = nullptr,
    };

    auto const result = _device.native().createPipelineLayout(layout_info);
    if(result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to create pipeline layout: '{}'",
                     vk::to_string(result.result));
        return;
    }

    _layout = result.value;
    BTX_TRACE("Created pipeline layout {:#x}",
              reinterpret_cast<uint64_t>(VkPipelineLayout(_layout)));
}


} // namespace btx