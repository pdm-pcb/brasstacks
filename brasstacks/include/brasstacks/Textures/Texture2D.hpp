#ifndef BRASSTACKS_TEXTURES_TEXTURE2D_HPP
#define BRASSTACKS_TEXTURES_TEXTURE2D_HPP

#include <cstdint>

namespace btx {

class Texture2D {
public:
	virtual void bind(const uint32_t slot = 0) = 0;
	virtual void unbind() const = 0;

    static Texture2D * create(const char *filepath, const bool inverted,
                              const bool gen_mipmaps);

    virtual ~Texture2D() = default;

    Texture2D(const Texture2D &&) = delete;
    Texture2D(Texture2D &)        = delete;

    Texture2D & operator=(const Texture2D &&) = delete;
    Texture2D & operator=(Texture2D &)        = delete;

protected:
    Texture2D() = default;

};

} // namespace btx

#endif // BRASSTACKS_TEXTURES_TEXTURE2D_HPP