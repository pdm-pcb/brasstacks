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
        "Push Constant Block:"
        "\n\toffset: {}"
        "\n\tsize: {}"
        "\n\tflags: {:s}",
        _push_constants.offset,
        _push_constants.size,
        vk::to_string(_push_constants.stageFlags)
    );
}

// =============================================================================
void vkShaderProgram::_merge_desc_set_bindings() {
    // for (const Shader& rShader : _rShaders)
    // {
    //     for (const VkDescriptorSetLayoutBinding& rLayoutBinding : rShader.layoutBindings)
    //     {
    //         if (tryUpdateBindingShaderStage(mergedLayoutBindings, rLayoutBinding))
    //         {
    //             continue;
    //         }

    //         mergedLayoutBindings.push_back(rLayoutBinding);
    //     }
    // }

    for(auto const &stage : _stages) {
        for(auto const &set_binding : stage.desc_set_bindings()) {

        }
    }
}

} // namespace btx