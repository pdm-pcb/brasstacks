#ifndef BRASSTACKS_PLATFORM_VULKAN_PIPELINE_VKSHADERSTAGE_HPP
#define BRASSTACKS_PLATFORM_VULKAN_PIPELINE_VKSHADERSTAGE_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkShaderStage final {
public:
    explicit vkShaderStage(std::string_view const filepath);
    ~vkShaderStage();

    inline auto const & native() const { return _handle; }
    inline auto stage() const { return _stage; }
    inline std::string_view const entry_point() const { return _entry_point; }

    vkShaderStage() = delete;

    vkShaderStage(vkShaderStage &&rhs) = delete;
    vkShaderStage(const vkShaderStage &) = delete;

    vkShaderStage& operator=(vkShaderStage &&) = delete;
    vkShaderStage& operator=(const vkShaderStage &) = delete;

private:
    vk::ShaderModule _handle;
    vk::Device _device;

    vk::ShaderStageFlagBits _stage;
    std::string _entry_point;
    std::vector<vk::VertexInputBindingDescription> _input_bindings;
    std::vector<vk::VertexInputAttributeDescription> _input_attribs;
    std::vector<vk::PushConstantRange> _push_constants;
    std::vector<vk::DescriptorSetLayoutBinding> _desc_set_bindings;

    using StringData = std::vector<char>;
    static StringData _spirv_to_string(std::filesystem::path const &filepath);

    using BinaryData = std::vector<uint32_t>;
    static BinaryData _string_to_binary(StringData const &shader_string);

    void _reflect_shader(StringData const &shader_string);
    bool _get_stage(::SpvReflectShaderModule const &module);
    bool _get_inputs(::SpvReflectShaderModule const &module);
    bool _get_push_constants(::SpvReflectShaderModule const &module);
    bool _get_descriptors(::SpvReflectShaderModule const &module);

    static vk::Format _get_format(::SpvReflectFormat const format);

    static vk::DescriptorType
    _get_descriptor_type(::SpvReflectDescriptorType const type);
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_PIPELINE_VKSHADERSTAGE_HPP