#include "brasstacks/System/pch.hpp"
#include "brasstacks/Textures/Texture2D.hpp"

#ifdef BTX_OPENGL
    #include "brasstacks/Platform/GL/Textures/GLTexture2D.hpp"
#elif defined(BTX_DIRECTX11)
    // ...?
#endif

namespace btx {

Texture2D * Texture2D::create(const char *filepath, const bool flip_vertical,
                              const bool gen_mipmaps,
                              const MinFilter min_filter,
                              const MagFilter mag_filter,
                              const Wrap wrap_s, const Wrap wrap_t) {
#ifdef PDR_DIRECTX11
    return new DX11Texture2D(filepath, flip_vertical, gen_mipmaps, min_filter,
                             mag_filter, wrap_s, wrap_t);
#else
    return new GLTexture2D(filepath, flip_vertical, gen_mipmaps, min_filter,
                           mag_filter, wrap_s, wrap_t);
#endif
}

} // namespace btx