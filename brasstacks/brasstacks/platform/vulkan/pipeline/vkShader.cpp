#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkShader.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"

#include <spirv_reflect.h>

namespace btx {

// =============================================================================
vkShader::vkShader() :
    _handle      { nullptr },
    _device      { nullptr },
    _stage       { },
    _entry_point { },
    _bindings    { }
{ }

// =============================================================================
vkShader::~vkShader() {
    if(_handle) {
        destroy();
    }
}

// =============================================================================
vkShader::vkShader(vkShader &&rhs) :
    _handle      { rhs._handle },
    _device      { rhs._device },
    _stage       { rhs._stage },
    _entry_point { rhs._entry_point },
    _bindings    { rhs._bindings }
{
    rhs._handle = nullptr;
    rhs._device = nullptr;
    rhs._stage = { };
    rhs._entry_point = "";
    rhs._bindings.clear();
}

// =============================================================================
void vkShader::create(std::string_view const filepath) {
    if(_handle) {
        BTX_CRITICAL("Shader {} already exists", _handle);
        return;
    }

    _device = Renderer::device().native();

    // Here we're just accounting for the path and filename differences
    // between debug and release builds
    std::filesystem::path shader_path = BTX_ASSET_PATH / filepath.data();
    shader_path += BTX_SHADER_EXT;

    BTX_TRACE("Reading SPIRV '{}'", shader_path.string());

    auto const shader_string = _spirv_to_string(shader_path);

    _reflect_shader(shader_string);

    auto const shader_binary = _string_to_binary(shader_string);

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
vkShader::StringData
vkShader::_spirv_to_string(std::filesystem::path const &filepath) {
    // Open the SPIR-V binary file and place the "cursor" at the end
    std::ifstream input_file(
        filepath.native(),
        std::ios::binary | std::ios::ate
    );

    if(!input_file.good()) {
        BTX_CRITICAL("Unable to open SPIRV '{}'", filepath.string());
        return StringData { };
    }

    // Since we're already at the end, std::ifstream::tellg() will give us the
    // file's size. Capture that, then seek back to the beginning in order to
    // begin reading the file's content properly.
    auto filesize = static_cast<size_t>(input_file.tellg());
    input_file.seekg(0, std::ifstream::beg);

    // Start by reading in an array of characters
    StringData shader_string(filesize);
    input_file.read(shader_string.data(),
                    static_cast<std::streamsize>(filesize));
    input_file.close();

    return shader_string;
}

// =============================================================================
vkShader::BinaryData
vkShader::_string_to_binary(StringData const &shader_string) {
    // Copy the character array into an integer array to provide Vulkan with
    // the binary data in the size and configuration it expects.
    BinaryData shader_binary(shader_string.size() / sizeof(uint32_t));
    ::memcpy(shader_binary.data(), shader_string.data(), shader_string.size());

    return shader_binary;
}

// =============================================================================
void vkShader::_reflect_shader(StringData const &shader_string) {
	SpvReflectShaderModule module { };
	auto result = ::spvReflectCreateShaderModule(
        shader_string.size(),
        shader_string.data(),
        &module
    );

    if(result != SPV_REFLECT_RESULT_SUCCESS) {
        BTX_CRITICAL("SPIRV-reflect failed to create shader module with "
                     "error code {}", result);
        return;
    }

    _stage = _get_shader_stage(module.shader_stage);
    _entry_point = module.entry_point_name;

	uint32_t binding_count = 0u;
	result = ::spvReflectEnumerateDescriptorBindings(
        &module,
        &binding_count,
        nullptr
    );

    if(result != SPV_REFLECT_RESULT_SUCCESS) {
        BTX_CRITICAL("SPIRV-reflect failed to get descriptor binding count "
                     "with error code {}", result);
        return;
    }

	std::vector<SpvReflectDescriptorBinding *> bindings;
    bindings.reserve(binding_count);

	result = ::spvReflectEnumerateDescriptorBindings(
        &module,
        &binding_count,
        bindings.data()
    );

    if(result != SPV_REFLECT_RESULT_SUCCESS) {
        BTX_CRITICAL("SPIRV-reflect failed to enumerate descriptor binding "
                     "with error code {}", result);
        return;
    }

	for(uint32_t i = 0u; i < binding_count; ++i) {
        auto const &set_layout_binding = module.descriptor_bindings[i];
        auto const type =
            _get_descriptor_type(set_layout_binding.descriptor_type);

        _bindings.emplace_back(vk::DescriptorSetLayoutBinding {
            .binding            = set_layout_binding.binding,
            .descriptorType     = type,
            .descriptorCount    = 1u,
            .stageFlags         = _stage,
            .pImmutableSamplers = nullptr,
        });
	}

	::spvReflectDestroyShaderModule(&module);
}

// =============================================================================
vk::ShaderStageFlagBits
vkShader::_get_shader_stage(SpvReflectShaderStageFlagBits const stage)
{
    if(stage == SPV_REFLECT_SHADER_STAGE_VERTEX_BIT) {
        return vk::ShaderStageFlagBits::eVertex;
    }
    if(stage == SPV_REFLECT_SHADER_STAGE_TESSELLATION_CONTROL_BIT) {
        return vk::ShaderStageFlagBits::eTessellationControl;
    }
    if(stage == SPV_REFLECT_SHADER_STAGE_TESSELLATION_EVALUATION_BIT) {
        return vk::ShaderStageFlagBits::eTessellationEvaluation;
    }
    if(stage == SPV_REFLECT_SHADER_STAGE_GEOMETRY_BIT) {
        return vk::ShaderStageFlagBits::eGeometry;
    }
    if(stage == SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT) {
        return vk::ShaderStageFlagBits::eFragment;
    }
    if(stage == SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT) {
        return vk::ShaderStageFlagBits::eCompute;
    }
    if(stage == SPV_REFLECT_SHADER_STAGE_TASK_BIT_EXT) {
        return vk::ShaderStageFlagBits::eTaskEXT;
    }
    if(stage == SPV_REFLECT_SHADER_STAGE_MESH_BIT_EXT) {
        return vk::ShaderStageFlagBits::eMeshEXT;
    }
    if(stage == SPV_REFLECT_SHADER_STAGE_RAYGEN_BIT_KHR) {
        return vk::ShaderStageFlagBits::eRaygenKHR;
    }
    if(stage == SPV_REFLECT_SHADER_STAGE_ANY_HIT_BIT_KHR) {
        return vk::ShaderStageFlagBits::eAnyHitKHR;
    }
    if(stage == SPV_REFLECT_SHADER_STAGE_CLOSEST_HIT_BIT_KHR) {
        return vk::ShaderStageFlagBits::eClosestHitKHR;
    }
    if(stage == SPV_REFLECT_SHADER_STAGE_MISS_BIT_KHR) {
        return vk::ShaderStageFlagBits::eMissKHR;
    }
    if(stage == SPV_REFLECT_SHADER_STAGE_INTERSECTION_BIT_KHR) {
        return vk::ShaderStageFlagBits::eIntersectionKHR;
    }
    if(stage == SPV_REFLECT_SHADER_STAGE_CALLABLE_BIT_KHR) {
        return vk::ShaderStageFlagBits::eCallableKHR;
    }

    BTX_CRITICAL("Unsupported SPIRV-reflect shader stage: {}", stage);
    return;
}

// =============================================================================
vk::DescriptorType
vkShader::_get_descriptor_type(SpvReflectDescriptorType const type)
{
    if(type == SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER) {
        return vk::DescriptorType::eSampler;
    }
    if(type == SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
        return vk::DescriptorType::eCombinedImageSampler;
    }
    if(type == SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE) {
        return vk::DescriptorType::eSampledImage;
    }
    if(type == SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE) {
        return vk::DescriptorType::eStorageImage;
    }
    if(type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER) {
        return vk::DescriptorType::eUniformTexelBuffer;
    }
    if(type == SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER) {
        return vk::DescriptorType::eStorageTexelBuffer;
    }
    if(type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
        return vk::DescriptorType::eUniformBuffer;
    }
    if(type == SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER) {
        return vk::DescriptorType::eStorageBuffer;
    }
    if(type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC) {
        return vk::DescriptorType::eUniformBufferDynamic;
    }
    if(type == SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC) {
        return vk::DescriptorType::eStorageBufferDynamic;
    }
    if(type == SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT) {
        return vk::DescriptorType::eInputAttachment;
    }
    if(type == SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR) {
        return vk::DescriptorType::eAccelerationStructureKHR;
    }

    BTX_CRITICAL("Unsupported SPIRV-reflect descriptor type: {}", type);
    return { };
}

} // namespace btx