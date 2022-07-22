#ifndef BRASSTACKS_PLATFORM_GL_TEXTURES_GLTEXTURE2D_HPP
#define BRASSTACKS_PLATFORM_GL_TEXTURES_GLTEXTURE2D_HPP

#include "brasstacks/Textures/Texture2D.hpp"

namespace btx {

class GLTexture2D : public Texture2D {
public:
	void bind(const uint32_t slot = 0) override;
	void unbind() const override;

    GLTexture2D(const char *filepath, const bool inverted,
                const bool gen_mipmaps);
    ~GLTexture2D();

    GLTexture2D() = delete;

    GLTexture2D(const GLTexture2D &&) = delete;
    GLTexture2D(GLTexture2D &)        = delete;

    GLTexture2D & operator=(const GLTexture2D &&) = delete;
    GLTexture2D & operator=(GLTexture2D &)        = delete;

private:
	GLenum _handle;
	GLenum _slot;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_GL_TEXTURES_GLTEXTURE2D_HPP