#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkShaderStage.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"

#include <spirv_reflect.h>

namespace btx {

// =============================================================================
vkShaderStage::vkShaderStage(std::string_view const filepath) :
    _handle            { nullptr },
    _device            { Renderer::device().native() },
    _stage             { },
    _entry_point       { },
    _desc_set_bindings { }
{
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
    BTX_TRACE("Created vkShaderStage module {} from '{}'", _handle, filepath);
}

// =============================================================================
vkShaderStage::~vkShaderStage() {
    BTX_TRACE("Destroying shader module {}", _handle);
    _device.destroyShaderModule(_handle);
    _handle = nullptr;
}

// =============================================================================
vkShaderStage::StringData
vkShaderStage::_spirv_to_string(std::filesystem::path const &filepath) {
    // Open the SPIR-V binary file and place the "cursor" at the end
    std::ifstream input_file(filepath.native(),
                             std::ios::binary | std::ios::ate);

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
vkShaderStage::BinaryData
vkShaderStage::_string_to_binary(StringData const &shader_string) {
    // Copy the character array into an integer array to provide Vulkan with
    // the binary data in the size and configuration it expects.
    BinaryData shader_binary(shader_string.size() / sizeof(uint32_t));
    ::memcpy(shader_binary.data(), shader_string.data(), shader_string.size());

    return shader_binary;
}

// =============================================================================
void vkShaderStage::_reflect_shader(StringData const &shader_string) {
	::SpvReflectShaderModule module { };
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

    // Gather the basics
    _entry_point = module.entry_point_name;

    if(!_get_stage(module)) {
        ::spvReflectDestroyShaderModule(&module);
        return;
    }

    // Gather input details
    if(module.input_variable_count > 0u && !_get_inputs(module)) {
        ::spvReflectDestroyShaderModule(&module);
        return;
    }

    // Gather push constants
    if(module.push_constant_block_count > 0u && !_get_push_constants(module)) {
        ::spvReflectDestroyShaderModule(&module);
        return;
    }

    // Gather descriptors
    if(module.descriptor_binding_count > 0u && !_get_descriptors(module)) {
        ::spvReflectDestroyShaderModule(&module);
        return;
    }

	::spvReflectDestroyShaderModule(&module);
}

// =============================================================================
bool vkShaderStage::_get_stage(::SpvReflectShaderModule const &module)
{
    if(module.shader_stage == SPV_REFLECT_SHADER_STAGE_VERTEX_BIT) {
        _stage = vk::ShaderStageFlagBits::eVertex;
        return true;
    }
    if(module.shader_stage == SPV_REFLECT_SHADER_STAGE_TESSELLATION_CONTROL_BIT) {
        _stage = vk::ShaderStageFlagBits::eTessellationControl;
        return true;
    }
    if(module.shader_stage == SPV_REFLECT_SHADER_STAGE_TESSELLATION_EVALUATION_BIT) {
        _stage = vk::ShaderStageFlagBits::eTessellationEvaluation;
        return true;
    }
    if(module.shader_stage == SPV_REFLECT_SHADER_STAGE_GEOMETRY_BIT) {
        _stage = vk::ShaderStageFlagBits::eGeometry;
        return true;
    }
    if(module.shader_stage == SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT) {
        _stage = vk::ShaderStageFlagBits::eFragment;
        return true;
    }
    if(module.shader_stage == SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT) {
        _stage = vk::ShaderStageFlagBits::eCompute;
        return true;
    }

    BTX_CRITICAL("Unsupported SPIRV-reflect shader stage: {:#x}",
                 module.shader_stage);

    return false;
}

// =============================================================================
bool vkShaderStage::_get_inputs(::SpvReflectShaderModule const &module) {
    _input_attribs.reserve(module.input_variable_count);

    uint32_t initial_offset = module.input_variables[0]->word_offset.location;

    for(uint32_t i = 0u; i < module.input_variable_count; ++i) {
        auto const *input = module.input_variables[i];

        _input_attribs.emplace_back(vk::VertexInputAttributeDescription {
            .location = input->location,
            .binding = 0u,
            .format = _get_format(input->format),
            .offset = input->word_offset.location - initial_offset
        });

        BTX_TRACE(
            "\n{:s} Input {}"
            "\n\tlocation: {}"
            "\n\tbinding: {}"
            "\n\tformat: {:s}"
            "\n\toffset: {}",
            vk::to_string(_stage),
            _input_attribs.size(),
            _input_attribs.back().location,
            _input_attribs.back().binding,
            vk::to_string(_input_attribs.back().format),
            _input_attribs.back().offset
        );
    }

    return true;
}

// =============================================================================
bool vkShaderStage::_get_push_constants(::SpvReflectShaderModule const &module)
{
    // This has got to be wrong... right?
    _push_constants.emplace_back(vk::PushConstantRange {
        .stageFlags = _stage,
        .offset = module.push_constant_blocks->offset,
        .size = module.push_constant_blocks->size
    });

    BTX_TRACE(
        "\n{:s} Push Constant {}"
        "\n\toffset: {}"
        "\n\tsize: {}",
        vk::to_string(_push_constants.back().stageFlags),
        _push_constants.size(),
        _push_constants.back().offset,
        _push_constants.back().size
    );

    return true;
}

// =============================================================================
bool vkShaderStage::_get_descriptors(::SpvReflectShaderModule const &module)
{
	uint32_t binding_count = 0u;
	auto result = ::spvReflectEnumerateDescriptorBindings(
        &module,
        &binding_count,
        nullptr
    );

    if(result != ::SPV_REFLECT_RESULT_SUCCESS) {
        BTX_CRITICAL("SPIRV-reflect failed to get descriptor binding count "
                     "with error code {}", result);
        return false;
    }

    if(binding_count != module.descriptor_binding_count) {
        BTX_CRITICAL("SPIRV-reflect first reported {} descriptor bindings, "
                     "but now reports {} descriptor bindings.",
                     module.descriptor_binding_count,
                     binding_count);
        return false;
    }

	std::vector<SpvReflectDescriptorBinding *> bindings(binding_count);

	result = ::spvReflectEnumerateDescriptorBindings(
        &module,
        &binding_count,
        bindings.data()
    );

    if(result != ::SPV_REFLECT_RESULT_SUCCESS) {
        BTX_CRITICAL("SPIRV-reflect failed to enumerate descriptor bindings "
                     "with error code {}", result);
        return false;
    }

    for(auto const *binding : bindings) {
        _desc_set_bindings.emplace_back(vk::DescriptorSetLayoutBinding {
            .binding = binding->binding,
            .descriptorType = _get_descriptor_type(binding->descriptor_type),
            .descriptorCount = 1u,
            .stageFlags = _stage,
            .pImmutableSamplers = nullptr
        });

        BTX_TRACE(
            "\n{:s} Descriptor set binding {} ({})"
            "\n\ttype: {:s}"
            "\n\tcount: {}",
            vk::to_string(_desc_set_bindings.back().stageFlags),
            _desc_set_bindings.size(),
            _desc_set_bindings.back().binding,
            vk::to_string(_desc_set_bindings.back().descriptorType),
            _desc_set_bindings.back().descriptorCount
        );
    }

    return true;
}

// =============================================================================
vk::Format vkShaderStage::_get_format(::SpvReflectFormat const format) {
    if(format == SPV_REFLECT_FORMAT_R32_UINT) {
        return vk::Format::eR32Uint;
    }
    if(format == SPV_REFLECT_FORMAT_R32_SINT) {
        return vk::Format::eR32Sint;
    }
    if(format == SPV_REFLECT_FORMAT_R32_SFLOAT) {
        return vk::Format::eR32Sfloat;
    }
    if(format == SPV_REFLECT_FORMAT_R32G32_UINT) {
        return vk::Format::eR32G32Uint;
    }
    if(format == SPV_REFLECT_FORMAT_R32G32_SINT) {
        return vk::Format::eR32G32Sint;
    }
    if(format == SPV_REFLECT_FORMAT_R32G32_SFLOAT) {
        return vk::Format::eR32G32Sfloat;
    }
    if(format == SPV_REFLECT_FORMAT_R32G32B32_UINT) {
        return vk::Format::eR32G32B32Uint;
    }
    if(format == SPV_REFLECT_FORMAT_R32G32B32_SINT) {
        return vk::Format::eR32G32B32Sint;
    }
    if(format == SPV_REFLECT_FORMAT_R32G32B32_SFLOAT) {
        return vk::Format::eR32G32B32Sfloat;
    }
    if(format == SPV_REFLECT_FORMAT_R32G32B32A32_UINT) {
        return vk::Format::eR32G32B32A32Uint;
    }
    if(format == SPV_REFLECT_FORMAT_R32G32B32A32_SINT) {
        return vk::Format::eR32G32B32A32Sint;
    }
    if(format == SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT) {
        return vk::Format::eR32G32B32A32Sfloat;
    }

    BTX_CRITICAL("Unsupported SPIRV-reflect format: {}", format);
    return { };
}

// =============================================================================
vk::DescriptorType
vkShaderStage::_get_descriptor_type(::SpvReflectDescriptorType const type)
{
    if(type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
        return vk::DescriptorType::eUniformBuffer;
    }
    if(type == SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
        return vk::DescriptorType::eCombinedImageSampler;
    }

    BTX_CRITICAL("Unsupported SPIRV-reflect descriptor type: {}", type);
    return { };
}

} // namespace btx