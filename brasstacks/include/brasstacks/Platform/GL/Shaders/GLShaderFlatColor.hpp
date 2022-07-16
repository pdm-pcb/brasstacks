#ifndef BRASSTACKS_PLATFORM_GL_SHADERS_GLSHADERFLATCOLOR_HPP
#define BRASSTACKS_PLATFORM_GL_SHADERS_GLSHADERFLATCOLOR_HPP

#include "brasstacks/Platform/GL/Shaders/GLShader.hpp"

namespace btx {

class GLShaderFlatColor : public GLShader {
public:
    GLShaderFlatColor();
    ~GLShaderFlatColor() = default;

    GLShaderFlatColor(const GLShaderFlatColor &&) = delete;
    GLShaderFlatColor(GLShaderFlatColor &)        = delete;

    GLShaderFlatColor & operator=(const GLShaderFlatColor &&) = delete;
    GLShaderFlatColor & operator=(GLShaderFlatColor &)        = delete;
};

} // namespacec btx

#endif // BRASSTACKS_PLATFORM_GL_SHADERS_GLSHADERFLATCOLOR_HPP