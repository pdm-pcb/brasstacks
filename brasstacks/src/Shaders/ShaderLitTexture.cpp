#include "brasstacks/System/pch.hpp"
#include "brasstacks/Shaders/ShaderLitTexture.hpp"

#include "brasstacks/ECS/Components/RenderComp.hpp"

namespace btx {

void ShaderLitTexture::update_camera(const glm::mat4 &view,
                                     const glm::mat4 &projection) const {
    _shader->update_camera(view, projection);
}

void ShaderLitTexture::update_render_data(const RenderComp &render_c) const {
    _shader->set_world_and_material(
        static_cast<RenderCompPhongNormalMap>(render_c).world_and_material
    );
}

ShaderLitTexture::ShaderLitTexture() {
#ifdef PDR_DIRECTX11
    _shader = new DX11ShaderLitTexture();
#else
    _shader = new GLShaderLitTexture();
#endif
}

ShaderLitTexture::~ShaderLitTexture() {
    delete _shader;
}

} // namespace btx