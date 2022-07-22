#ifndef BRASSTACKS_SHADERS_SHADERFLATTEXTURE_HPP
#define BRASSTACKS_SHADERS_SHADERFLATTEXTURE_HPP

#include "brasstacks/Shaders/Shader.hpp"

#ifdef BTX_OPENGL
    #include "brasstacks/Platform/GL/Shaders/GLShaderFlatTexture.hpp"
#elif defined(BTX_DIRECTX11)
    // ...?
#endif

#include <concepts>

namespace btx {

class ShaderFlatTexture : public Shader {
public:
    void bind()   const override { _shader->bind();   }
    void unbind() const override { _shader->unbind(); }

    void update_camera(const glm::mat4 &view,
                       const glm::mat4 &projection) const override;

    void update_render_data(const RenderComp &render_c) const override;

    ShaderFlatTexture();
    ~ShaderFlatTexture();

    ShaderFlatTexture(const ShaderFlatTexture &) = delete;
    ShaderFlatTexture(ShaderFlatTexture &&)      = delete;

    ShaderFlatTexture & operator=(const ShaderFlatTexture &) = delete;
    ShaderFlatTexture & operator=(ShaderFlatTexture &&)      = delete;

private:

#ifdef BTX_OPENGL
    GLShaderFlatTexture   *_shader;
#elif defined(BTX_DIRECTX11)
    DX11ShaderFlatTexture *_shader;
#endif
};

} // namespace btx

#endif // BRASSTACKS_SHADERS_SHADERFLATTEXTURE_HPP