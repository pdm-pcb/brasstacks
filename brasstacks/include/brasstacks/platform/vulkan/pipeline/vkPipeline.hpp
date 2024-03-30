#ifndef BRASSTACKS_PLATFORM_VULKAN_PIPELINE_VKPIPELINE_HPP
#define BRASSTACKS_PLATFORM_VULKAN_PIPELINE_VKPIPELINE_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/config/RenderConfig.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkShader.hpp"

namespace btx {

class vkRenderPassBase;
class vkCmdBuffer;
class vkDescriptorSet;
class vkDescriptorSetLayout;

class vkPipeline final {
public:
    vkPipeline();
    ~vkPipeline();

    static vk::SampleCountFlagBits samples_to_flag(uint32_t const samples);

    vkPipeline & module_from_spirv(std::string_view const filepath,
                                   vk::ShaderStageFlagBits const stage,
                                   std::string_view const entry_point = "main");

    using VertBindings = std::vector<vk::VertexInputBindingDescription>;
    using VertAttribs  = std::vector<vk::VertexInputAttributeDescription>;
    vkPipeline & describe_vertex_input(VertBindings const &bindings,
                                       VertAttribs const &attributes);

    vkPipeline & add_descriptor_set_layout(vkDescriptorSetLayout const &layout);

    struct PushConstant {
        vk::ShaderStageFlags const stage_flags = vk::ShaderStageFlagBits::eAll;
        size_t const size_bytes = 0;
        void const *data = nullptr;
    };

    vkPipeline & add_push_constant(PushConstant const &push_constant);

    struct Config {
        // Attachment configuration
        std::vector<vk::Format> color_formats;
        vk::Format depth_format = vk::Format::eUndefined;

        // Viewport settings
        RenderConfig::Size   viewport_extent { 0u, 0u };
        RenderConfig::Offset viewport_offset { 0, 0 };

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

    void create(vkRenderPassBase const &render_pass, Config const &config);
    void destroy();

    void bind(vkCmdBuffer const &cmd_buffer);
    void bind_descriptor_set(vkDescriptorSet const &set) const;
    void unbind();

    void send_push_constants(std::span<PushConstant> const push_constants);

    void update_dimensions(RenderConfig::Size const &size,
                           RenderConfig::Offset const &offset);

    inline auto const & native()   const { return _handle; }
    inline auto const & layout()   const { return _layout; }
    inline auto const & viewport() const { return _viewport; }
    inline auto const & scissor()  const { return _scissor; }

    vkPipeline(vkPipeline &&) = delete;
    vkPipeline(const vkPipeline &) = delete;

    vkPipeline& operator=(vkPipeline &&) = delete;
    vkPipeline& operator=(const vkPipeline &) = delete;

private:
    vk::Pipeline       _handle;
    vk::PipelineLayout _layout;

    vk::Device _device;

    std::vector<std::unique_ptr<vkShader>>         _shaders;
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

    std::vector<vk::DescriptorSetLayout>   _set_layouts;
    std::unordered_map<uint64_t, uint32_t> _set_bind_points;

    std::vector<vk::PushConstantRange> _push_constants;
    size_t _push_constant_offset;

    vkCmdBuffer const *_cmd_buffer;

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