#include "brasstacks/System/pch.hpp"
#include "brasstacks/Shaders/ShaderFlatColor.hpp"

namespace btx {

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