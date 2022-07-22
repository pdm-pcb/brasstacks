#ifndef BRASSTACKS_PLATFORM_GL_SHADERS_GLSHADER_HPP
#define BRASSTACKS_PLATFORM_GL_SHADERS_GLSHADER_HPP

#include "brasstacks/Shaders/Shader.hpp"

#include <cstdint>
#include <array>

namespace btx {

class GLShader {
public:
    void bind();
    static void unbind();

    void update_camera(const glm::mat4 &view, const glm::mat4 &proj);

    GLShader();
    virtual ~GLShader();

    GLShader(const GLShader &other)             = delete;
    GLShader(GLShader &&other)                  = delete;

    GLShader & operator=(const GLShader &other) = delete;
    GLShader & operator=(GLShader &&other)      = delete;

protected:
    void create_cam_ubo();
    void add_program(const char *path, const Shader::Type type);
    void link_program();

    GLuint handle() const { return _handle; }

private:
    GLuint _handle;
    GLuint _vert;
    GLuint _frag;
    GLuint _geo;

    GLuint _cam_ubo;

    void _print_shader_log() const; 
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_GL_SHADERS_GLSHADER_HPP