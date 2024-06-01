#ifndef BRASSTACKS_PLATFORM_VULKAN_PIPELINE_VKSHADER_HPP
#define BRASSTACKS_PLATFORM_VULKAN_PIPELINE_VKSHADER_HPP

#include "brasstacks/pch.hpp"

enum SpvReflectShaderStageFlagBits;
enum SpvReflectDescriptorType;

namespace btx {

class vkShader final {
public:
    vkShader();
    ~vkShader();

    void create(std::string_view const filepath);
    void destroy();

    inline auto const & native() const { return _handle; }
    inline auto const stage() const { return _stage; }
    inline std::string_view const entry_point() const { return _entry_point; }
    inline auto const & bindings() const { return _bindings; }

    vkShader(vkShader &&rhs);
    vkShader(const vkShader &) = delete;

    vkShader& operator=(vkShader &&) = delete;
    vkShader& operator=(const vkShader &) = delete;

private:
    vk::ShaderModule _handle;
    vk::Device _device;

    vk::ShaderStageFlagBits _stage;
    std::string _entry_point;
    std::vector<vk::DescriptorSetLayoutBinding const> _bindings;

    using StringData = std::vector<char>;
    static StringData _spirv_to_string(std::filesystem::path const &filepath);

    using BinaryData = std::vector<uint32_t>;
    static BinaryData _string_to_binary(StringData const &shader_string);

    void _reflect_shader(StringData const &shader_string);

    static vk::ShaderStageFlagBits
    _get_shader_stage(SpvReflectShaderStageFlagBits const stage);

    static vk::DescriptorType
    _get_descriptor_type(SpvReflectDescriptorType const type);
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_PIPELINE_VKSHADER_HPP