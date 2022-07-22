#include "brasstacks/System/pch.hpp"
#include "brasstacks/Shaders/ShaderLitTexture.hpp"

#include "brasstacks/ECS/Components/cRender.hpp"

namespace btx {

void ShaderLitTexture::update_camera(const glm::mat4 &view,
                                     const glm::mat4 &projection) const {
    _shader->update_camera(view, projection);
}

void ShaderLitTexture::update_render_data(const Entity::ID id) const {
    auto phong = ECS::get_active()->get<cPhongNormalMap>(id);
    _shader->set_world_and_material(phong->world_and_material);
    _shader->set_light_params(phong->light_params);
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