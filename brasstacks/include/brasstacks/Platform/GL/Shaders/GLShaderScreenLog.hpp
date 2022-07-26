#ifndef BRASSTACKS_PLATFORM_GL_SHADERS_GLSHADERSCREENLOG_HPP
#define BRASSTACKS_PLATFORM_GL_SHADERS_GLSHADERSCREENLOG_HPP

#include "brasstacks/Platform/GL/Shaders/GLShader.hpp"

namespace btx {

class GLShaderScreenLog : public GLShader {
public:
    void set_text_color(const glm::vec4 &text_color) const;

    GLShaderScreenLog();
    ~GLShaderScreenLog() = default;

    GLShaderScreenLog(const GLShaderScreenLog &&) = delete;
    GLShaderScreenLog(GLShaderScreenLog &)        = delete;

    GLShaderScreenLog & operator=(const GLShaderScreenLog &&) = delete;
    GLShaderScreenLog & operator=(GLShaderScreenLog &)        = delete;

private:
    GLuint _text_color;

    void _find_uniform();
};

} // namespacec btx

#endif // BRASSTACKS_PLATFORM_GL_SHADERS_GLSHADERSCREENLOG_HPP