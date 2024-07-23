#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkShaderProgram.hpp"

#include "brasstacks/platform/vulkan/pipeline/vkShaderStage.hpp"

#include <spirv_reflect.h>

namespace btx {

// =============================================================================
vkShaderProgram::vkShaderProgram() :
    _stages { },
    _push_constants { }
{ }

// =============================================================================
vkShaderProgram::~vkShaderProgram() {

}

// =============================================================================
vkShaderProgram & vkShaderProgram::add_stage(std::string_view const filepath) {
    _stages.emplace_back(filepath);
    return *this;
}

// =============================================================================
void vkShaderProgram::create() {
    _merge_push_constants();
    _merge_descriptor_sets();
}

// =============================================================================
void vkShaderProgram::destroy() {
}

// =============================================================================
void vkShaderProgram::_merge_push_constants() {
    for(auto const &stage : _stages) {
        auto const &block = stage.push_constants();
        if(block.stageFlags) {
            if(_push_constants.size == 0u) {
                _push_constants.offset = block.offset;
                _push_constants.size = block.size;
            }

            assert(_push_constants.size == block.size);
            assert(_push_constants.offset == block.offset);

            _push_constants.stageFlags |= block.stageFlags;
        }
    }

    BTX_TRACE(
        "\nPush Constant Block:"
        "\n\toffset: {}"
        "\n\tsize: {}"
        "\n\tstages: {:s}",
        _push_constants.offset,
        _push_constants.size,
        vk::to_string(_push_constants.stageFlags)
    );
}

// =============================================================================
void vkShaderProgram::_merge_descriptor_sets() {
    // for(auto const &stage : _stages) {
    //     for(auto const &binding : stage.desc_set_bindings()) {
    //         if(_update_binding_stage(binding)) {
    //             continue;
    //         }

    //         _desc_set_bindings.push_back(binding);
    //     }
    // }

    // for(auto const &binding : _desc_set_bindings) {
    //     BTX_TRACE(
    //         "\nDescriptor set binding ({})"
    //         "\n\ttype: {:s}"
    //         "\n\tcount: {}"
    //         "\n\tstages: {:s}",
    //         binding.binding,
    //         vk::to_string(binding.descriptorType),
    //         binding.descriptorCount,
    //         vk::to_string(binding.stageFlags)
    //     );
    // }
}

// =============================================================================
bool vkShaderProgram::_update_binding_stage(
    vk::DescriptorSetLayoutBinding const &binding)
{
    // for(auto &merged_binding : _desc_set_bindings) {
    //     if(merged_binding.binding == binding.binding) {
    //         merged_binding.stageFlags |= binding.stageFlags;
    //         return true;
    //     }
    // }

    return false;
}

} // namespace btx