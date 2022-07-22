#include "brasstacks/System/pch.hpp"
#include "brasstacks/Shaders/ShaderLitTexture.hpp"

#include "brasstacks/ECS/Components/cRender.hpp"

namespace btx {

void ShaderLitTexture::update_camera(const glm::mat4 &view,
                                     const glm::mat4 &projection) const {
    _shader->update_camera(view, projection);
}

void ShaderLitTexture::update_per_frame() const {
    ECS *ecs = ECS::get_active();
    auto lights = ecs->get<cPhongParams>(_per_frame_id);

    _shader->set_light_params(lights->params);
}

void ShaderLitTexture::update_per_object(const Entity::ID id) const {
    ECS *ecs = ECS::get_active();
    auto world    = ecs->get<cWorldMat>(id);
    auto material = ecs->get<cPhongMaterial>(id);

    _shader->set_world_and_material({
        world->world_mat,
        material->ambient,
        material->diffuse,
        material->specular,
        material->shine
    });
}

ShaderLitTexture::ShaderLitTexture() :
    _per_frame_id { static_cast<Entity::ID>(-1) }
{
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