#include "brasstacks/System/pch.hpp"
#include "brasstacks/Platform/GL/Shaders/GLShaderFlatColor.hpp"

#include "brasstacks/Shaders/Shader.hpp"

namespace btx {

GLShaderFlatColor::GLShaderFlatColor() {
    add_program("../../assets/shaders/glsl/flat_color.vert", Type::Vertex);
    add_program("../../assets/shaders/glsl/flat_color.frag", Type::Fragment);
    link_program();
}

} // namespace btx