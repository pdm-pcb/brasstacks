#ifndef BRASSTACKS_TEXTURES_TEXTURE2D_HPP
#define BRASSTACKS_TEXTURES_TEXTURE2D_HPP

#include <cstdint>

namespace btx {

class Texture2D {
public:
    enum class MinFilter {
        nearest, linear,
        neatest_mipmap_nearest,
        linear_mipmap_nearest,
        nearest_mipmap_linear,
        linear_mipmap_lienar,
    };

    enum class MagFilter {
        nearest, linear,
    };

    enum class Wrap {
        clamp_to_edge,
        clamp_to_border,
        mirror_repeat,
        repeat,
        mirror_clamp_to_edge,
    };

	virtual void bind(const uint32_t slot = 0) = 0;
	virtual void unbind() const = 0;

    static Texture2D * create(const char *filepath, const bool flip_vertical,
                              const bool gen_mipmaps,
                              const MinFilter min_filter,
                              const MagFilter mag_filter,
                              const Wrap wrap_s, const Wrap wrap_t);

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