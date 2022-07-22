#ifndef BRASSTACKS_PLATFORM_GL_SHADERS_GLSHADERFLATTEXTURE_HPP
#define BRASSTACKS_PLATFORM_GL_SHADERS_GLSHADERFLATTEXTURE_HPP

#include "brasstacks/Platform/GL/Shaders/GLShader.hpp"

namespace btx {

class GLShaderLitTexture : public GLShader {
public:
    void set_world_and_material(const Shader::WorldAndMaterial &params);
    void set_light_params(const Shader::LightParameters &params);

    GLShaderLitTexture();
    ~GLShaderLitTexture() = default;

    GLShaderLitTexture(const GLShaderLitTexture &&) = delete;
    GLShaderLitTexture(GLShaderLitTexture &)        = delete;

    GLShaderLitTexture & operator=(const GLShaderLitTexture &&) = delete;
    GLShaderLitTexture & operator=(GLShaderLitTexture &)        = delete;

private:
    GLuint _world_material_ubo;
    GLuint _light_param_ubo;

    void _create_ubos();
};

} // namespacec btx

#endif // BRASSTACKS_PLATFORM_GL_SHADERS_GLSHADERFLATTEXTURE_HPP