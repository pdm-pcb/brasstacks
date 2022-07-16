#include "brasstacks/System/pch.hpp"
#include "brasstacks/Shaders/ShaderFlatColor.hpp"

namespace btx {

void ShaderFlatColor::update_camera(const glm::mat4 &view,
                                    const glm::mat4 &projection) const {
    _shader->update_camera(view, projection);
}

void ShaderFlatColor::set_world(const glm::mat4 &world) const {
    _shader->set_world(world);
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