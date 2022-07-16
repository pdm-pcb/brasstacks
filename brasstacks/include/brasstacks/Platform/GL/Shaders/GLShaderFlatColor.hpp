#ifndef BRASSTACKS_PLATFORM_GL_SHADERS_GLSHADERFLATCOLOR_HPP
#define BRASSTACKS_PLATFORM_GL_SHADERS_GLSHADERFLATCOLOR_HPP

#include "brasstacks/Platform/GL/Shaders/GLShader.hpp"

namespace btx {

class GLShaderFlatColor : public GLShader {
public:
    void set_world(const glm::mat4 &world) const;

    GLShaderFlatColor();
    ~GLShaderFlatColor() = default;

    GLShaderFlatColor(const GLShaderFlatColor &&) = delete;
    GLShaderFlatColor(GLShaderFlatColor &)        = delete;

    GLShaderFlatColor & operator=(const GLShaderFlatColor &&) = delete;
    GLShaderFlatColor & operator=(GLShaderFlatColor &)        = delete;

private:
    GLuint _world_uniform;

    void _find_uniforms();
};

} // namespacec btx

#endif // BRASSTACKS_PLATFORM_GL_SHADERS_GLSHADERFLATCOLOR_HPP