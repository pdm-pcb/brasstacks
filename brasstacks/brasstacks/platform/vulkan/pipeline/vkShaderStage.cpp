#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkShaderStage.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"

#include <spirv_reflect.h>

namespace btx {

// =============================================================================
vkShaderStage::vkShaderStage(std::string_view const filepath) :
    _handle         { nullptr },
    _device         { Renderer::device().native() },
    _stage          { },
    _entry_point    { },
    _desc_set_layout_bindings { }
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

    // // Gather the basics
    // if(!_reflect_stage(module)) {
    //     ::spvReflectDestroyShaderModule(&module);
    //     return;
    // }

    // _entry_point = module.entry_point_name;

    // // Gather input details
    // if(!_reflect_input(module)) {
    //     ::spvReflectDestroyShaderModule(&module);
    //     return;
    // }

    // // Gather descriptor sets and bindings
    // if(!_reflect_descriptors(module)) {
    //     ::spvReflectDestroyShaderModule(&module);
    //     return;
    // }

    // // Gather push constants
    // if(!_reflect_push_constants(module)) {
    //     ::spvReflectDestroyShaderModule(&module);
    //     return;
    // }

	// ::spvReflectDestroyShaderModule(&module);
}

// // =============================================================================
// bool vkShaderStage::_reflect_stage(::SpvReflectShaderModule const &module)
// {
//     if(module.shader_stage == SPV_REFLECT_SHADER_STAGE_VERTEX_BIT) {
//         _stage = vk::ShaderStageFlagBits::eVertex;
//         return true;
//     }
//     if(module.shader_stage == SPV_REFLECT_SHADER_STAGE_TESSELLATION_CONTROL_BIT) {
//         _stage = vk::ShaderStageFlagBits::eTessellationControl;
//         return true;
//     }
//     if(module.shader_stage == SPV_REFLECT_SHADER_STAGE_TESSELLATION_EVALUATION_BIT) {
//         _stage = vk::ShaderStageFlagBits::eTessellationEvaluation;
//         return true;
//     }
//     if(module.shader_stage == SPV_REFLECT_SHADER_STAGE_GEOMETRY_BIT) {
//         _stage = vk::ShaderStageFlagBits::eGeometry;
//         return true;
//     }
//     if(module.shader_stage == SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT) {
//         _stage = vk::ShaderStageFlagBits::eFragment;
//         return true;
//     }
//     if(module.shader_stage == SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT) {
//         _stage = vk::ShaderStageFlagBits::eCompute;
//         return true;
//     }

//     BTX_CRITICAL("Unsupported SPIRV-reflect shader stage: {:#x}",
//                  module.shader_stage);

//     return false;
// }

// // =============================================================================
// bool vkShaderStage::_reflect_input(::SpvReflectShaderModule const &module) {
//     uint32_t input_count = 0u;
//     auto result = ::spvReflectEnumerateInputVariables(
//         &module,
//         &input_count,
//         nullptr
//     );

//     if(result != ::SPV_REFLECT_RESULT_SUCCESS) {
//         BTX_CRITICAL("SPIRV-reflect failed to get input variable count with "
//                      "error code {}", result);
//     }

//     std::vector<SpvReflectInterfaceVariable *> inputs(input_count);

//     result = ::spvReflectEnumerateInputVariables(
//         &module,
//         &input_count,
//         inputs.data()
//     );

//     if(result != ::SPV_REFLECT_RESULT_SUCCESS) {
//         BTX_CRITICAL("SPIRV-reflect failed to enumerate input variables with "
//                      "error code {}", result);
//         return false;
//     }

//     uint32_t initial_offset = inputs[0]->word_offset.location;

//     for(auto const *input : inputs) {
//         _input_attribs.emplace_back(vk::VertexInputAttributeDescription {
//             .location = input->location,
//             .binding = 0u,
//             .format = _get_input_format(input->format),
//             .offset = input->word_offset.location - initial_offset
//         });

//         BTX_INFO(
//             "\n\tlocation: {}"
//             "\n\tbinding: {}"
//             "\n\tformat: {}"
//             "\n\toffset: {}",
//             _input_attribs.back().location,
//             _input_attribs.back().binding,
//             vk::to_string(_input_attribs.back().format),
//             _input_attribs.back().offset
//         );
//     }

//     for(auto const &attrib : Vertex::attributes) {
//         BTX_WARN(
//             "\n\tlocation: {}"
//             "\n\tbinding: {}"
//             "\n\tformat: {}"
//             "\n\toffset: {}",
//             attrib.location,
//             attrib.binding,
//             vk::to_string(attrib.format),
//             attrib.offset
//         );
//     }

//     return true;
// }

// // =============================================================================
// bool vkShaderStage::_reflect_descriptors(::SpvReflectShaderModule const &module)
// {
// 	uint32_t binding_count = 0u;
// 	auto result = ::spvReflectEnumerateDescriptorBindings(
//         &module,
//         &binding_count,
//         nullptr
//     );

//     if(result != ::SPV_REFLECT_RESULT_SUCCESS) {
//         BTX_CRITICAL("SPIRV-reflect failed to get descriptor binding count "
//                      "with error code {}", result);
//         return false;
//     }

// 	std::vector<SpvReflectDescriptorBinding *> bindings(binding_count);

// 	result = ::spvReflectEnumerateDescriptorBindings(
//         &module,
//         &binding_count,
//         bindings.data()
//     );

//     if(result != ::SPV_REFLECT_RESULT_SUCCESS) {
//         BTX_CRITICAL("SPIRV-reflect failed to enumerate descriptor bindings "
//                      "with error code {}", result);
//         return false;
//     }

//     for(auto const *binding : bindings) {
//         _desc_set_layout_bindings.emplace_back(vk::DescriptorSetLayoutBinding {
//             .binding = binding->binding,
//             .descriptorType = _get_descriptor_type(binding->descriptor_type),
//             .descriptorCount = 1u,
//             .stageFlags = _stage,
//             .pImmutableSamplers = nullptr
//         });
//     }

//     return true;
// }

// // =============================================================================
// bool
// vkShaderStage::_reflect_push_constants(::SpvReflectShaderModule const &module)
// {
//     uint32_t push_constant_count = 0u;
//     auto result = ::spvReflectEnumeratePushConstantBlocks(
//         &module,
//         &push_constant_count,
//         nullptr
//     );

//     if(result != ::SPV_REFLECT_RESULT_SUCCESS) {
//         BTX_CRITICAL("SPIRV-reflect failed to get push constant block count "
//                      "with error code {}", result);
//         return false;
//     }

//     std::vector<::SpvReflectBlockVariable *>
//         push_constants(push_constant_count);

//     result = ::spvReflectEnumeratePushConstantBlocks(
//         &module,
//         &push_constant_count,
//         push_constants.data()
//     );

//     if(result != ::SPV_REFLECT_RESULT_SUCCESS) {
//         BTX_CRITICAL("SPIRV-reflect failed to enumerate push constant blocks "
//                      "with error code {}", result);
//         return false;
//     }

//     for(auto const *push_constant : push_constants) {
//         _push_constants.push_back(vk::PushConstantRange {
//             .stageFlags = _stage,
//             .offset = push_constant->offset,
//             .size = push_constant->size
//         });
//     }

//     return true;
// }

// // =============================================================================
// vk::Format vkShaderStage::_get_input_format(::SpvReflectFormat const format) {
//     if(format == SPV_REFLECT_FORMAT_R32_UINT) {
//         return vk::Format::eR32Uint;
//     }
//     if(format == SPV_REFLECT_FORMAT_R32_SINT) {
//         return vk::Format::eR32Sint;
//     }
//     if(format == SPV_REFLECT_FORMAT_R32_SFLOAT) {
//         return vk::Format::eR32Sfloat;
//     }
//     if(format == SPV_REFLECT_FORMAT_R32G32_UINT) {
//         return vk::Format::eR32G32Uint;
//     }
//     if(format == SPV_REFLECT_FORMAT_R32G32_SINT) {
//         return vk::Format::eR32G32Sint;
//     }
//     if(format == SPV_REFLECT_FORMAT_R32G32_SFLOAT) {
//         return vk::Format::eR32G32Sfloat;
//     }
//     if(format == SPV_REFLECT_FORMAT_R32G32B32_UINT) {
//         return vk::Format::eR32G32B32Uint;
//     }
//     if(format == SPV_REFLECT_FORMAT_R32G32B32_SINT) {
//         return vk::Format::eR32G32B32Sint;
//     }
//     if(format == SPV_REFLECT_FORMAT_R32G32B32_SFLOAT) {
//         return vk::Format::eR32G32B32Sfloat;
//     }
//     if(format == SPV_REFLECT_FORMAT_R32G32B32A32_UINT) {
//         return vk::Format::eR32G32B32A32Uint;
//     }
//     if(format == SPV_REFLECT_FORMAT_R32G32B32A32_SINT) {
//         return vk::Format::eR32G32B32A32Sint;
//     }
//     if(format == SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT) {
//         return vk::Format::eR32G32B32A32Sfloat;
//     }

//     BTX_CRITICAL("Unsupported SPIRV-reflect format: {}", format);
//     return { };
// }

// // =============================================================================
// vk::DescriptorType
// vkShaderStage::_get_descriptor_type(::SpvReflectDescriptorType const type)
// {
//     if(type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
//         return vk::DescriptorType::eUniformBuffer;
//     }
//     if(type == SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
//         return vk::DescriptorType::eCombinedImageSampler;
//     }

//     BTX_CRITICAL("Unsupported SPIRV-reflect descriptor type: {}", type);
//     return { };
// }

} // namespace btx