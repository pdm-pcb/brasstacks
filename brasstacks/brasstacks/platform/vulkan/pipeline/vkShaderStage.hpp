#ifndef BRASSTACKS_PLATFORM_VULKAN_PIPELINE_VKSHADERSTAGE_HPP
#define BRASSTACKS_PLATFORM_VULKAN_PIPELINE_VKSHADERSTAGE_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkShaderStage final {
public:
    explicit vkShaderStage(std::string_view const filepath);
    ~vkShaderStage();

    inline auto const & native() const { return _handle; }
    inline auto const & stage() const { return _stage; }
    inline std::string_view const entry_point() const { return _entry_point; }
    inline auto const & push_constants() const { return _push_constants; }
    inline auto const & descriptor_sets() const { return _descriptor_sets; }

    vkShaderStage() = delete;

    vkShaderStage(vkShaderStage &&rhs);
    vkShaderStage(vkShaderStage const &) = delete;

    vkShaderStage& operator=(vkShaderStage &&) = delete;
    vkShaderStage& operator=(vkShaderStage const &) = delete;

private:
    vk::ShaderModule _handle;
    vk::Device _device;

    vk::ShaderStageFlagBits _stage;
    std::string _entry_point;
    std::vector<vk::VertexInputBindingDescription> _input_bindings;
    std::vector<vk::VertexInputAttributeDescription> _input_attribs;
    std::vector<vk::PushConstantRange> _push_constants;

    struct DescriptorSetInfo {
        uint32_t set_number;
        std::vector<vk::DescriptorSetLayoutBinding> bindings;
    };
    std::vector<DescriptorSetInfo> _descriptor_sets;

    using StringData = std::vector<char>;
    static StringData _spirv_to_string(std::filesystem::path const &filepath);

    using BinaryData = std::vector<uint32_t>;
    static BinaryData _string_to_binary(StringData const &shader_string);

    void _reflect_shader(StringData const &shader_string);
    void _get_stage(::SpvReflectShaderModule const &module);
    void _get_entry_point(::SpvReflectShaderModule const &module);
    void _get_inputs(::SpvReflectShaderModule const &module);
    void _get_push_constants(::SpvReflectShaderModule const &module);
    void _get_descriptor_sets(::SpvReflectShaderModule const &module);

    static vk::Format _get_format(::SpvReflectFormat const format);

    static vk::DescriptorType
    _get_descriptor_type(::SpvReflectDescriptorType const type);
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_PIPELINE_VKSHADERSTAGE_HPP