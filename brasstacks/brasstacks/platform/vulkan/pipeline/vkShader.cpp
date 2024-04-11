#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkShader.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"

namespace btx {

// =============================================================================
vkShader::vkShader() :
    _handle { nullptr },
    _device { nullptr }
{ }

// =============================================================================
vkShader::~vkShader() {
    if(_handle) {
        destroy();
    }
}

// =============================================================================
vkShader::vkShader(vkShader &&rhs) :
    _handle { rhs._handle },
    _device { rhs._device }
{
    rhs._handle = nullptr;
    rhs._device = nullptr;
}

// =============================================================================
void vkShader::create(std::string_view const filepath) {
    if(_handle) {
        BTX_CRITICAL("Shader {} already exists", _handle);
        return;
    }

    _device = Renderer::device().native();

    auto const shader_binary = _spirv_to_binary(filepath);
    const vk::ShaderModuleCreateInfo module_info {
        .codeSize = shader_binary.size() * sizeof(uint32_t),
        .pCode = shader_binary.data(),
    };

    _handle = _device.createShaderModule(module_info);
    BTX_TRACE("Created vkShader module {} from '{}'", _handle, filepath);
}

// =============================================================================
void vkShader::destroy() {
    BTX_TRACE("Destroying shader module {}", _handle);
    _device.destroyShaderModule(_handle);
    _handle = nullptr;
}

// =============================================================================
vkShader::BinaryData vkShader::_spirv_to_binary(std::string_view filepath) {
    // Here we're just accounting for the path and filename differences
    // between debug and release builds
    std::filesystem::path shader_path = BTX_ASSET_PATH / filepath.data();
    shader_path += BTX_SHADER_EXT;

    // Open the SPIR-V binary file and place the "cursor" at the end
    std::ifstream input_file(
        shader_path.native(),
        std::ios::binary | std::ios::ate
    );

    if(!input_file.good()) {
        BTX_CRITICAL("Unable to open SPIRV '{}'", shader_path.string());
        return BinaryData { };
    }

    // Since we're already at the end, std::ifstream::tellg() will give us the
    // file's size. Capture that, then seek back to the beginning in order to
    // begin reading the file's content properly.
    auto filesize = static_cast<size_t>(input_file.tellg());
    input_file.seekg(0, std::ifstream::beg);

    // Start by reading in an array of characters
    StringData shader_string(filesize);
    input_file.read(
        shader_string.data(),
        static_cast<std::streamsize>(filesize)
    );
    input_file.close();

    // Then copy the character array into an integer array to provide Vulkan
    // with the binary data in the size and configuration it expects.
    BinaryData shader_binary(shader_string.size() / sizeof(uint32_t));
    ::memcpy(shader_binary.data(), shader_string.data(), shader_string.size());

    return shader_binary;
}

} // namespace btx