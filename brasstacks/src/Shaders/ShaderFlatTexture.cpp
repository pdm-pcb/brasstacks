#include "brasstacks/System/pch.hpp"
#include "brasstacks/Shaders/ShaderFlatTexture.hpp"

#include "brasstacks/ECS/Components/RenderComp.hpp"

namespace btx {

void ShaderFlatTexture::update_camera(const glm::mat4 &view,
                                    const glm::mat4 &projection) const {
    _shader->update_camera(view, projection);
}

void ShaderFlatTexture::update_render_data(const RenderComp &render_c) const {
    _shader->set_world(render_c.world_mat);
}

ShaderFlatTexture::ShaderFlatTexture() {
#ifdef PDR_DIRECTX11
    _shader = new DX11ShaderFlatTexture();
#else
    _shader = new GLShaderFlatTexture();
#endif
}

ShaderFlatTexture::~ShaderFlatTexture() {
    delete _shader;
}

} // namespace btx