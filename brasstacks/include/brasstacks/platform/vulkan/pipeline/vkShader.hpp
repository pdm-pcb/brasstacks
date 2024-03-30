#ifndef BRASSTACKS_PLATFORM_VULKAN_PIPELINE_VKSHADER_HPP
#define BRASSTACKS_PLATFORM_VULKAN_PIPELINE_VKSHADER_HPP

namespace btx {

class vkDevice;

class vkShader final {
public:
    inline auto const & native() const { return _handle; }

    explicit vkShader(std::string_view const filepath);
    ~vkShader();

    vkShader() = delete;

    vkShader(vkShader &&) = delete;
    vkShader(const vkShader &) = delete;

    vkShader& operator=(vkShader &&) = delete;
    vkShader& operator=(const vkShader &) = delete;

private:
    vk::ShaderModule _handle;

    using BinaryData = std::vector<uint32_t>;
    using StringData = std::vector<char>;
    static BinaryData _spirv_to_binary(std::string_view filepath);
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_PIPELINE_VKSHADER_HPP