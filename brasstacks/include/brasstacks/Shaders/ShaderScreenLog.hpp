#ifndef BRASSTACKS_SHADERS_SHADERSCREENLOG_HPP
#define BRASSTACKS_SHADERS_SHADERSCREENLOG_HPP

#include "brasstacks/Shaders/Shader.hpp"

#ifdef BTX_OPENGL
    #include "brasstacks/Platform/GL/Shaders/GLShaderScreenLog.hpp"
#elif defined(BTX_DIRECTX11)
    // ...?
#endif

#include <concepts>

namespace btx {

class ShaderScreenLog : public Shader {
public:
    void bind()   const override { _shader->bind();   }
    void unbind() const override { _shader->unbind(); }

    void update_camera(const glm::mat4 &view,
                       const glm::mat4 &projection) const override;

    void update_per_frame() const override { }
    void update_per_object([[maybe_unused]] const Entity::ID id) const override
    { }

    ShaderScreenLog();
    ~ShaderScreenLog();

    ShaderScreenLog(const ShaderScreenLog &) = delete;
    ShaderScreenLog(ShaderScreenLog &&)      = delete;

    ShaderScreenLog & operator=(const ShaderScreenLog &) = delete;
    ShaderScreenLog & operator=(ShaderScreenLog &&)      = delete;

private:

#ifdef BTX_OPENGL
    GLShaderScreenLog   *_shader;
#elif defined(BTX_DIRECTX11)
    DX11ShaderScreenLog *_shader;
#endif
};

} // namespace btx

#endif // BRASSTACKS_SHADERS_SHADERSCREENLOG_HPP