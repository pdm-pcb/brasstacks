#include "brasstacks/System/pch.hpp"
#include "brasstacks/Platform/GL/Shaders/GLShaderFlatColor.hpp"

#include "brasstacks/Shaders/Shader.hpp"

namespace btx {

void GLShaderFlatColor::set_world(const glm::mat4 &world) {
    glUniformMatrix4fv(_world_uniform, 1, GL_FALSE, &world[0][0]);
}

void GLShaderFlatColor::_find_uniforms() {
    _world_uniform = glGetUniformLocation(handle(), "world_matrix");
}

GLShaderFlatColor::GLShaderFlatColor() :
    _world_uniform { GL_NONE }
{
    add_program("../../assets/shaders/glsl/flat_color.vert", Type::Vertex);
    add_program("../../assets/shaders/glsl/flat_color.frag", Type::Fragment);
    link_program();
    create_cam_ubo();
    _find_uniforms();
}

} // namespace btx