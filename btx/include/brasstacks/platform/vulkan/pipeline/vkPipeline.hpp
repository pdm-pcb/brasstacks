#ifndef BRASSTACKS_PLATFORM_VULKAN_PIPELINE_VKPIPELINE_HPP
#define BRASSTACKS_PLATFORM_VULKAN_PIPELINE_VKPIPELINE_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDevice;
class vkShader;
class vkRenderPass;
class vkCmdBuffer;

class vkPipeline final {
public:
    explicit vkPipeline(vkDevice const &device);

    ~vkPipeline();

    struct Config {
        // Attachment configuration
        std::vector<vk::Format> color_formats;
        vk::Format depth_format = vk::Format::eUndefined;

        // Viewport settings
        vk::Extent2D viewport_extent { 0u, 0u };
        vk::Offset2D viewport_offset { 0, 0 };

        // Drawing options
        vk::PolygonMode   polygon_mode = vk::PolygonMode::eFill;
        vk::CullModeFlags cull_mode    = vk::CullModeFlagBits::eBack;
        vk::FrontFace     front_face   = vk::FrontFace::eClockwise;

        // Multisample config
        vk::SampleCountFlagBits sample_flags = vk::SampleCountFlagBits::e1;

        // Depth/shadow buffer options
        vk::Bool32    enable_depth_test   = VK_FALSE;
        vk::CompareOp depth_compare       = vk::CompareOp::eLess;
        vk::Bool32    enable_depth_bias   = VK_FALSE;
        float         depth_bias_constant = 0.0f;
        float         depth_bias_slope    = 0.0f;

        // Universal options
        uint32_t subpass_index = 0u;
    };

    void create(vkRenderPass const &render_pass, Config const &config);

    void bind(vkCmdBuffer const &cmd_buffer);

    vkPipeline & module_from_spirv(std::string_view filepath,
                                   vk::ShaderStageFlagBits const stage,
                                   std::string_view entry_point = "main");

    using VertBindings = std::vector<vk::VertexInputBindingDescription>;
    using VertAttribs  = std::vector<vk::VertexInputAttributeDescription>;
    vkPipeline & describe_vertex_input(VertBindings const &bindings,
                                       VertAttribs const &attributes);

    void update_dimensions(vk::Extent2D const &extent,
                           vk::Offset2D const &offset);

    inline auto const& native()   const { return _handle; }
    inline auto const& viewport() const { return _viewport; }
    inline auto const& scissor()  const { return _scissor; }

    vkPipeline(vkPipeline &&) = delete;
    vkPipeline(const vkPipeline &) = delete;

    vkPipeline& operator=(vkPipeline &&) = delete;
    vkPipeline& operator=(const vkPipeline &) = delete;

private:
    vkDevice const &_device;

    std::vector<vkShader *>                        _shaders;
    std::vector<vk::PipelineShaderStageCreateInfo> _shader_stages;

    vk::Viewport _viewport;
    vk::Rect2D   _scissor;

    vk::PipelineVertexInputStateCreateInfo   _vert_input_info;
    vk::PipelineInputAssemblyStateCreateInfo _assembly_info;
    vk::PipelineViewportStateCreateInfo      _viewport_info;
    vk::PipelineRasterizationStateCreateInfo _raster_info;
    vk::PipelineMultisampleStateCreateInfo   _multisample_info;
    vk::PipelineDepthStencilStateCreateInfo  _depth_stencil_info;

    std::vector<vk::PipelineColorBlendAttachmentState> _blend_states;
    vk::PipelineColorBlendStateCreateInfo _blend_info;

    std::vector<vk::DynamicState>            _dynamic_states;
    vk::PipelineDynamicStateCreateInfo       _dynamic_state_info;

    vk::PipelineLayout _layout;

    vk::Pipeline _handle;

    void _init_assembly();
    void _init_viewport(Config const &config);
    void _init_raster(Config const &config);
    void _init_multisample(Config const &config);
    void _init_depth_stencil(Config const &config);
    void _init_blend_states();
    void _init_dynamic_states();
    void _init_layout();
};

} // namespace btx;

#endif // BRASSTACKS_PLATFORM_VULKAN_PIPELINE_VKPIPELINE_HPP