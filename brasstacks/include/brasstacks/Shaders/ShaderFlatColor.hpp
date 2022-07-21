#ifndef BRASSTACKS_SHADERS_SHADERFLATCOLOR_HPP
#define BRASSTACKS_SHADERS_SHADERFLATCOLOR_HPP

#include "brasstacks/Shaders/Shader.hpp"

#ifdef BTX_OPENGL
    #include "brasstacks/Platform/GL/Shaders/GLShaderFlatColor.hpp"
#elif defined(BTX_DIRECTX11)
    // ...?
#endif

#include <concepts>

namespace btx {

class ShaderFlatColor : public Shader {
public:
    void bind()   const override { _shader->bind();   }
    void unbind() const override { _shader->unbind(); }

    void update_camera(const glm::mat4 &view,
                       const glm::mat4 &projection) const override;

    void update_render_data(const RenderComp &render_c) const override;

    ShaderFlatColor();
    ~ShaderFlatColor();

    ShaderFlatColor(const ShaderFlatColor &) = delete;
    ShaderFlatColor(ShaderFlatColor &&)      = delete;

    ShaderFlatColor & operator=(const ShaderFlatColor &) = delete;
    ShaderFlatColor & operator=(ShaderFlatColor &&)      = delete;

private:

#ifdef BTX_OPENGL
    GLShaderFlatColor   *_shader;
#elif defined(BTX_DIRECTX11)
    DX11ShaderFlatColor *_shader;
#endif
};

} // namespace btx

#endif // BRASSTACKS_SHADERS_SHADERFLATCOLOR_HPP