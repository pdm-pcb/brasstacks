#ifndef BRASSTACKS_SHADERS_SHADERLITTEXTURE_HPP
#define BRASSTACKS_SHADERS_SHADERLITTEXTURE_HPP

#include "brasstacks/Shaders/Shader.hpp"

#ifdef BTX_OPENGL
    #include "brasstacks/Platform/GL/Shaders/GLShaderLitTexture.hpp"
#elif defined(BTX_DIRECTX11)
    // ...?
#endif

#include <concepts>

namespace btx {

class ShaderLitTexture : public Shader {
public:
    void bind()   const override { _shader->bind();   }
    void unbind() const override { _shader->unbind(); }

    void update_camera(const glm::mat4 &view,
                       const glm::mat4 &projection) const override;

    void update_per_frame() const override;
    void update_per_object(const Entity::ID id) const override;

    void store_per_frame_id(const Entity::ID id) { _per_frame_id = id; }

    ShaderLitTexture();
    ~ShaderLitTexture();

    ShaderLitTexture(const ShaderLitTexture &) = delete;
    ShaderLitTexture(ShaderLitTexture &&)      = delete;

    ShaderLitTexture & operator=(const ShaderLitTexture &) = delete;
    ShaderLitTexture & operator=(ShaderLitTexture &&)      = delete;

private:
#ifdef BTX_OPENGL
    GLShaderLitTexture   *_shader;
#elif defined(BTX_DIRECTX11)
    DX11ShaderLitTexture *_shader;
#endif

    Entity::ID _per_frame_id;
};

} // namespace btx

#endif // BRASSTACKS_SHADERS_SHADERLITTEXTURE_HPP