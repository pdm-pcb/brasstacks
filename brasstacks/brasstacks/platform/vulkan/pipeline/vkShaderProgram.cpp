#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkShaderProgram.hpp"

#include "brasstacks/platform/vulkan/pipeline/vkShaderStage.hpp"

#include <spirv_reflect.h>

namespace btx {

// =============================================================================
vkShaderProgram::vkShaderProgram() :
    _device { nullptr },
    _stages { }
{ }

// =============================================================================
vkShaderProgram::~vkShaderProgram() {

}

// =============================================================================
vkShaderProgram & vkShaderProgram::add_stage(std::string_view const filepath) {
    // _stages.emplace_back(filepath);
    return *this;
}

// =============================================================================
void vkShaderProgram::create() {

}

} // namespace btx