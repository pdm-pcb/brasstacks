#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkShaderStage.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"

#include <spirv_reflect.h>

namespace btx {

// =============================================================================
vkShaderStage::vkShaderStage(std::string_view const filepath) :
    _handle          { nullptr },
    _device          { Renderer::device().native() },
    _stage           { },
    _entry_point     { },
    _input_bindings  { },
    _input_attribs   { },
    _push_constants  { },
    _descriptor_sets { }
{
    BTX_ERROR("==============================================================");
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
    BTX_ERROR("==============================================================");
}

// =============================================================================
vkShaderStage::~vkShaderStage() {
    if(_handle && _device) {
        BTX_TRACE("Destroying shader module {}", _handle);
        _device.destroyShaderModule(_handle);
        _handle = nullptr;
    }
}

// =============================================================================
vkShaderStage::vkShaderStage(vkShaderStage &&rhs) :
    _handle          { rhs._handle },
    _device          { rhs._device },
    _stage           { rhs._stage },
    _input_bindings  { rhs._input_bindings },
    _input_attribs   { rhs._input_attribs },
    _push_constants  { rhs._push_constants },
    _descriptor_sets { rhs._descriptor_sets }
{
    rhs._handle          = nullptr;
    rhs._device          = nullptr;
    rhs._stage           = { };
    rhs._input_bindings  = { };
    rhs._input_attribs   = { };
    rhs._push_constants  = { };
    rhs._descriptor_sets = { };
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

    // Gather the reflected details
    _get_stage(module);
    _get_entry_point(module);
    _get_inputs(module);
    _get_push_constants(module);
    _get_descriptor_sets(module);

	::spvReflectDestroyShaderModule(&module);
}

// =============================================================================
void vkShaderStage::_get_stage(::SpvReflectShaderModule const &module) {
    switch(module.shader_stage) {
        case SPV_REFLECT_SHADER_STAGE_VERTEX_BIT:
            _stage = vk::ShaderStageFlagBits::eVertex;
            break;

        case SPV_REFLECT_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
            _stage = vk::ShaderStageFlagBits::eTessellationControl;
            break;

        case SPV_REFLECT_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
            _stage = vk::ShaderStageFlagBits::eTessellationEvaluation;
            break;

        case SPV_REFLECT_SHADER_STAGE_GEOMETRY_BIT:
            _stage = vk::ShaderStageFlagBits::eGeometry;
            break;

        case SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT:
            _stage = vk::ShaderStageFlagBits::eFragment;
            break;

        case SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT:
            _stage = vk::ShaderStageFlagBits::eCompute;
            break;

        default:
            BTX_CRITICAL("Unsupported SPIRV-reflect shader stage: {:#x}",
                         module.shader_stage);
    }
}

// =============================================================================
void vkShaderStage::_get_entry_point(::SpvReflectShaderModule const &module) {
    if(module.entry_point_count != 1) {
        BTX_CRITICAL("Shader module has {} entry points; must only have one.",
                     module.entry_point_count);
        return;
    }

    _entry_point = std::string(module.entry_points[0].name);

    BTX_TRACE("Entry point: '{:s}'", _entry_point);
}

// =============================================================================
void vkShaderStage::_get_inputs(::SpvReflectShaderModule const &module) {
    if(module.input_variable_count == 0u) {
        return;
    }

    _input_attribs.reserve(module.input_variable_count);

    uint32_t initial_offset = module.input_variables[0]->word_offset.location;

    for(uint32_t input = 0u; input < module.input_variable_count; ++input) {
        auto const &refl_input = *(module.input_variables[input]);

        _input_attribs.emplace_back(vk::VertexInputAttributeDescription {
            .location = refl_input.location,
            .binding = 0u,
            .format = _get_format(refl_input.format),
            .offset = refl_input.word_offset.location - initial_offset
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
}

// =============================================================================
void vkShaderStage::_get_push_constants(::SpvReflectShaderModule const &module)
{
    if(module.push_constant_block_count == 0u) {
        return;
    }

    _push_constants.reserve(module.push_constant_block_count);

    for(uint32_t block = 0u; block < module.push_constant_block_count; ++block)
    {
        auto const &refl_block = module.push_constant_blocks[block];

        _push_constants.emplace_back(vk::PushConstantRange {
            .stageFlags = _stage,
            .offset = refl_block.offset,
            .size = refl_block.size,
        });

        BTX_TRACE(
            "\nPush Constant Block {}"
            "\n\toffset: {}"
            "\n\tsize: {}"
            "\n\tmember count: {}"
            "\n\tstage: {:s}",
            _push_constants.size(),
            _push_constants.back().offset,
            _push_constants.back().size,
            refl_block.member_count,
            vk::to_string(_push_constants.back().stageFlags)
        );
    }
}

// =============================================================================
void vkShaderStage::_get_descriptor_sets(::SpvReflectShaderModule const &module)
{
    if(module.descriptor_set_count == 0u) {
        return;
    }

    _descriptor_sets.reserve(module.descriptor_set_count);

    for(uint32_t set = 0u; set < module.descriptor_set_count; ++set) {
        _descriptor_sets.emplace_back();

        auto &local_set = _descriptor_sets.back();
        auto const &refl_set = module.descriptor_sets[set];

        local_set.set_number = refl_set.set;
        local_set.bindings.resize(refl_set.binding_count);

        for(uint32_t binding = 0u; binding < refl_set.binding_count; ++binding)
        {
            auto &local_binding = local_set.bindings[binding];
            auto const &refl_binding = *(refl_set.bindings[binding]);

            local_binding.binding = refl_binding.binding;

            local_binding.descriptorType =
                vk::DescriptorType(refl_binding.descriptor_type);

            local_binding.descriptorCount = 1u;
            for(uint32_t dim = 0u; dim < refl_binding.array.dims_count; ++dim) {
                local_binding.descriptorCount *= refl_binding.array.dims[dim];
            }

            local_binding.stageFlags = _stage;
        }
    }

    for(auto const &set : _descriptor_sets) {
        for(auto const &binding : set.bindings) {
            BTX_TRACE(
                "\nSet {}, Binding {}"
                "\n\ttype: {:s}"
                "\n\tcount: {}"
                "\n\tstage: {:s}",
                set.set_number,
                binding.binding,
                vk::to_string(binding.descriptorType),
                binding.descriptorCount,
                vk::to_string(binding.stageFlags)
            );
        }
    }
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