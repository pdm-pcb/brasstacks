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

    // auto dir = phong->light_params.directional_light;
    // BTX_ENGINE_TRACE(
    //     "Update for {}"
    //     "\n\tDirection: {:.02f},{:.02f},{:.02f},{:.02f}"
    //     "\n\tAmbient:   {:.02f},{:.02f},{:.02f},{:.02f}"
    //     "\n\tDiffuse:   {:.02f},{:.02f},{:.02f},{:.02f}"
    //     "\n\tSpecular:  {:.02f},{:.02f},{:.02f},{:.02f}"
    //     "\n\tShine:     {:.02f}", Entity::get_index(id),
    //     dir.direction.x, dir.direction.y, dir.direction.z, dir.direction.w,
    //     dir.props.ambient.r,  dir.props.ambient.g,  dir.props.ambient.b,  dir.props.ambient.a,
    //     dir.props.diffuse.r,  dir.props.diffuse.g,  dir.props.diffuse.b,  dir.props.diffuse.a,
    //     dir.props.specular.r, dir.props.specular.g, dir.props.specular.b, dir.props.specular.a,
    //     dir.props.shine
    // );
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