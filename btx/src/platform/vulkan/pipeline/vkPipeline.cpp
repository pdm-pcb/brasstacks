#include "brasstacks/brasstacks.hpp"

#include "brasstacks/platform/vulkan/pipeline/vkPipeline.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkShader.hpp"
#include "brasstacks/platform/vulkan/rendering/vkRenderPass.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorSet.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorSetLayout.hpp"

namespace btx {

// =============================================================================
vkPipeline::vkPipeline(vkDevice const &device) :
    _device               { device },
    _handle               { nullptr },
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
    _push_constants       { },
    _push_constant_offset { 0 },
    _layout               { },
    _cmd_buffer           { nullptr }
{ }

// =============================================================================
vkPipeline::~vkPipeline() {
    BTX_TRACE("Destroying pipeline {}, layout {}", _handle, _layout);

    _device.native().destroy(_layout);
    _device.native().destroy(_handle);
}

// =============================================================================
vk::SampleCountFlagBits vkPipeline::samples_to_flag(uint32_t const samples) {
    if(samples == 64u) { return vk::SampleCountFlagBits::e64; }
    if(samples == 32u) { return vk::SampleCountFlagBits::e32; }
    if(samples == 16u) { return vk::SampleCountFlagBits::e16; }
    if(samples == 8u)  { return vk::SampleCountFlagBits::e8;  }
    if(samples == 4u)  { return vk::SampleCountFlagBits::e4;  }
    if(samples == 2u)  { return vk::SampleCountFlagBits::e2;  }
    if(samples == 1u)  { return vk::SampleCountFlagBits::e1;  }

    BTX_CRITICAL("Unsupported MSAA sample count {}", samples);

    return { };
}

// =============================================================================
void vkPipeline::bind(vkCmdBuffer const &cmd_buffer) {
    if(_cmd_buffer != nullptr) {
        BTX_ERROR("Trying to bind already bound pipeline");
        return;
    }

    _cmd_buffer = &cmd_buffer;

    _cmd_buffer->native().bindPipeline(
        vk::PipelineBindPoint::eGraphics,
        _handle
    );
    _cmd_buffer->native().setViewport(0u, _viewport);
    _cmd_buffer->native().setScissor(0u, _scissor);
}

// =============================================================================
void vkPipeline::bind_descriptor_set(vkDescriptorSet const &set) const {
    if(_cmd_buffer == nullptr) {
        BTX_ERROR("Trying to bind descriptor set to unbound pipeline");
        return;
    }

    auto const set_layout_key = reinterpret_cast<uint64_t>(
        VkDescriptorSetLayout(set.layout().native())
    );

    _cmd_buffer->native().bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        _layout,
        _set_bind_points.at(set_layout_key),
        1u, &(set.native()),
        0u, nullptr
    );
}

// =============================================================================
void vkPipeline::unbind() {
    if(_cmd_buffer == nullptr) {
        BTX_ERROR("Cannot unbind pipeline twice");
        return;
    }

    _cmd_buffer = nullptr;
}

// =============================================================================
vkPipeline & vkPipeline::module_from_spirv(std::string_view const filepath,
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
vkPipeline &
vkPipeline::add_descriptor_set(vkDescriptorSetLayout const &layout) {
    if(_handle) {
        BTX_CRITICAL("Adding a descriptor set to a pipeline that's already "
                     "been created.");
    }

    _set_layouts.push_back(layout.native());
    return *this;
}

// =============================================================================
vkPipeline & vkPipeline::add_push_constant(PushConstant const &push_constant) {
    _push_constants.push_back({
        .stageFlags = push_constant.stage_flags,
        .offset = static_cast<uint32_t>(_push_constant_offset),
        .size = static_cast<uint32_t>(push_constant.size_bytes)
    });

    _push_constant_offset += push_constant.size_bytes;

    return *this;
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
    BTX_TRACE("Created Vulkan pipeline {}", _handle);

    // Destroy the shader modules now that the pipeline is baked
    for(auto *shader : _shaders) {
        delete shader;
    }
}

// =============================================================================
void vkPipeline::send_push_constants(PushConstants const &push_constants) {
    size_t offset = 0u;
    for(auto const& push_constant : push_constants) {
        _cmd_buffer->native().pushConstants(
            _layout,
            push_constant.stage_flags,
            static_cast<uint32_t>(offset),
            static_cast<uint32_t>(push_constant.size_bytes),
            push_constant.data
        );

        offset += push_constant.size_bytes;
    }
}

// =============================================================================
void vkPipeline::update_dimensions(RenderConfig::Size const &size,
                                   RenderConfig::Offset const &offset)
{
    _viewport = vk::Viewport {
        .x         = static_cast<float>(offset.x),
        .y         = static_cast<float>(size.height),
        .width     = static_cast<float>(size.width),
        .height    = -static_cast<float>(size.height),
        .minDepth  = 0.0f,
        .maxDepth  = 1.0f,
    };

    _scissor = vk::Rect2D {
        .offset = { .x = offset.x, .y = offset.y },
        .extent = { .width = size.width, .height = size.height },
    };

    BTX_TRACE(
        "Pipeline viewport updated: {:.02f} x {:.02f} ({:.02f}, {:.02f}) ",
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
    uint32_t set_binding_point = 0u;

    for(auto const &layout : _set_layouts) {
        auto const set_key = reinterpret_cast<uint64_t>(
            VkDescriptorSetLayout(layout)
        );

        _set_bind_points[set_key] = set_binding_point++;
    }

    const vk::PipelineLayoutCreateInfo layout_info {
        .setLayoutCount         = static_cast<uint32_t>(_set_layouts.size()),
        .pSetLayouts            = _set_layouts.data(),
        .pushConstantRangeCount = static_cast<uint32_t>(_push_constants.size()),
        .pPushConstantRanges    = _push_constants.data()
    };

    auto const result = _device.native().createPipelineLayout(layout_info);
    if(result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to create pipeline layout: '{}'",
                     vk::to_string(result.result));
        return;
    }

    _layout = result.value;
    BTX_TRACE("Created pipeline layout {}", _layout);
}


} // namespace btx