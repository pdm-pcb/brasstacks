#include "brasstacks/System/pch.hpp"
#include "brasstacks/Platform/GL/Shaders/GLShaderScreenLog.hpp"

#include "brasstacks/Shaders/Shader.hpp"

namespace btx {

void GLShaderScreenLog::set_projmat(const glm::mat4 &projmat) const {

    glUniformMatrix4fv(_proj_uniform, 1, GL_FALSE, &projmat[0][0]);
}

void GLShaderScreenLog::_find_uniform() {
    _proj_uniform = glGetUniformLocation(handle(), "projection");
}

GLShaderScreenLog::GLShaderScreenLog() :
    GLShader(),
    _proj_uniform { GL_NONE }
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