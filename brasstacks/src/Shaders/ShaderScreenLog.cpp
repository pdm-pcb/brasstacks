#include "brasstacks/System/pch.hpp"
#include "brasstacks/Shaders/ShaderScreenLog.hpp"

#include "brasstacks/ECS/Components/cRender.hpp"

namespace btx {

glm::vec4 black(0.0f, 0.0f, 0.0f, 1.0f);

void ShaderScreenLog::update_camera(const glm::mat4 &view,
                                    const glm::mat4 &projection) const {
    _shader->update_camera(view, projection);
    _shader->set_text_color(black);
}

ShaderScreenLog::ShaderScreenLog() {
#ifdef PDR_DIRECTX11
    _shader = new DX11ShaderScreenLog();
#else
    _shader = new GLShaderScreenLog();
#endif
}

ShaderScreenLog::~ShaderScreenLog() {
    delete _shader;
}

} // namespace btx