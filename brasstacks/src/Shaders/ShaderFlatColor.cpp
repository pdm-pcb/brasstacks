#include "brasstacks/System/pch.hpp"
#include "brasstacks/Shaders/ShaderFlatColor.hpp"

#include "brasstacks/ECS/Components/cRender.hpp"

namespace btx {

void ShaderFlatColor::update_camera(const glm::mat4 &view,
                                    const glm::mat4 &projection) const {
    _shader->update_camera(view, projection);
}

void ShaderFlatColor::update_render_data(const Entity::ID id) const {
    _shader->set_world(
        ECS::get_active()->get<cWorldMat>(id)->world_mat
    );
}

ShaderFlatColor::ShaderFlatColor() {
#ifdef PDR_DIRECTX11
    _shader = new DX11ShaderFlatColor();
#else
    _shader = new GLShaderFlatColor();
#endif
}

ShaderFlatColor::~ShaderFlatColor() {
    delete _shader;
}

} // namespace btx