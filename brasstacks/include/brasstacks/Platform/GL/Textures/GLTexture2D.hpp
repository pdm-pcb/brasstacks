#ifndef BRASSTACKS_PLATFORM_GL_TEXTURES_GLTEXTURE2D_HPP
#define BRASSTACKS_PLATFORM_GL_TEXTURES_GLTEXTURE2D_HPP

#include "brasstacks/Textures/Texture2D.hpp"

namespace btx {

class GLTexture2D : public Texture2D {
public:
	void bind(const uint32_t slot = 0) override;
    void set_data(const void *data, const GLint width,
                  const GLint height, const std::size_t channels) override;

    GLTexture2D(const char *filepath, const bool flip_vertical,
                const bool gen_mipmaps,
                const MinFilter min_filter, const MagFilter mag_filter,
                const Wrap wrap_s, const Wrap wrap_t);
    GLTexture2D(const bool gen_mipmaps,
                const MinFilter min_filter, const MagFilter mag_filter,
                const Wrap wrap_s, const Wrap wrap_t);
    ~GLTexture2D();

    GLTexture2D() = delete;

    GLTexture2D(const GLTexture2D &&) = delete;
    GLTexture2D(GLTexture2D &)        = delete;

    GLTexture2D & operator=(const GLTexture2D &&) = delete;
    GLTexture2D & operator=(GLTexture2D &)        = delete;

private:
	::GLuint _handle;
	::GLuint _slot;

    bool _has_mipmaps;

    void _set_params(const MinFilter min_filter, const MagFilter mag_filter,
                     const Wrap wrap_s, const Wrap wrap_t);
    void _set_channels(const std::size_t channels, GLenum &format,
                       GLenum &internal_format);
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_GL_TEXTURES_GLTEXTURE2D_HPP