#include "brasstacks/System/pch.hpp"
#include "brasstacks/Shaders/ShaderFlatTexture.hpp"

#include "brasstacks/ECS/Components/cRender.hpp"

namespace btx {

void ShaderFlatTexture::update_camera(const glm::mat4 &view,
                                      const glm::mat4 &projection) const {
    _shader->update_camera(view, projection);
}

void ShaderFlatTexture::update_per_object(const Entity::ID id) const {
    _shader->set_world(
        ECS::get_active()->get<cWorldMat>(id)->world_mat
    );
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