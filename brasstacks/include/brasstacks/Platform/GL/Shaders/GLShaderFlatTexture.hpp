#ifndef BRASSTACKS_PLATFORM_GL_SHADERS_GLSHADERFLATTEXTURE_HPP
#define BRASSTACKS_PLATFORM_GL_SHADERS_GLSHADERFLATTEXTURE_HPP

#include "brasstacks/Platform/GL/Shaders/GLShader.hpp"

namespace btx {

class GLShaderFlatTexture : public GLShader {
public:
    void set_world(const glm::mat4 &world) const;

    GLShaderFlatTexture();
    ~GLShaderFlatTexture() = default;

    GLShaderFlatTexture(const GLShaderFlatTexture &&) = delete;
    GLShaderFlatTexture(GLShaderFlatTexture &)        = delete;

    GLShaderFlatTexture & operator=(const GLShaderFlatTexture &&) = delete;
    GLShaderFlatTexture & operator=(GLShaderFlatTexture &)        = delete;

private:
    GLuint _world_uniform;

    void _find_uniform();
};

} // namespacec btx

#endif // BRASSTACKS_PLATFORM_GL_SHADERS_GLSHADERFLATTEXTURE_HPP