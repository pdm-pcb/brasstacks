#include "brasstacks/System/pch.hpp"
#include "brasstacks/Platform/GL/Shaders/GLShaderScreenLog.hpp"

#include "brasstacks/Shaders/Shader.hpp"

namespace btx {

void GLShaderScreenLog::set_text_color(const glm::vec4 &text_color) const {
    glUniform4f(
        _text_color,
        text_color.x,
        text_color.y,
        text_color.z,
        text_color.w
    );
}

void GLShaderScreenLog::_find_uniform() {
    _text_color = glGetUniformLocation(handle(), "text_color");
}

GLShaderScreenLog::GLShaderScreenLog() :
    GLShader(),
    _text_color { GL_NONE }
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