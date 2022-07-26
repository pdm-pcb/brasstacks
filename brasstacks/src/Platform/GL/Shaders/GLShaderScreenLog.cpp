#include "brasstacks/System/pch.hpp"
#include "brasstacks/Platform/GL/Shaders/GLShaderScreenLog.hpp"

#include "brasstacks/Shaders/Shader.hpp"

namespace btx {

void GLShaderScreenLog::set_world(const glm::mat4 &world) const {
    glUniformMatrix4fv(_world_uniform, 1, GL_FALSE, &world[0][0]);
}

void GLShaderScreenLog::_find_uniform() {
    _world_uniform = glGetUniformLocation(handle(), "world_matrix");
}

GLShaderScreenLog::GLShaderScreenLog() :
    GLShader(),
    _world_uniform { GL_NONE }
{
    add_program("../../assets/shaders/glsl/screen_log.vert",
                Shader::Type::Vertex);
    add_program("../../assets/shaders/glsl/screen_log.frag",
                Shader::Type::Pixel);

    link_programs();

    create_cam_ubo();
    _find_uniform();
}

} // namespace btx