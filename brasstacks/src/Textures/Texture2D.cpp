#include "brasstacks/System/pch.hpp"
#include "brasstacks/Textures/Texture2D.hpp"

#ifdef BTX_OPENGL
    #include "brasstacks/Platform/GL/Textures/GLTexture2D.hpp"
#elif defined(BTX_DIRECTX11)
    // ...?
#endif

namespace btx {

Texture2D * Texture2D::create(const char *filepath, const bool inverted,
                              const bool gen_mipmaps) {
#ifdef PDR_DIRECTX11
    return new DX11Texture2D(filepath, inverted, gen_mipmaps);
#else
    return new GLTexture2D(filepath, inverted, gen_mipmaps);
#endif
}

} // namespace btx