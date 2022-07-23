#ifndef BRASSTACKS_ASSETLIBRARIES_TEXTURELIBRARY_HPP
#define BRASSTACKS_ASSETLIBRARIES_TEXTURELIBRARY_HPP

#include "brasstacks/Textures/Texture2D.hpp"

#include "unordered_map"
#include "string"

namespace btx {

class Texture2D;

class TextureLibrary final {
public:
    static void load(const char *path, const char *key,
                     const bool flip_vertical,
                     const bool gen_mipmaps,
                     const Texture2D::MinFilter min_filter,
                     const Texture2D::MagFilter mag_filter,
                     const Texture2D::Wrap wrap_s,
                     const Texture2D::Wrap wrap_t);

    static Texture2D * checkout(const char *key);

    static void shutdown();
    static void remove(const char *key);

    TextureLibrary() = delete;
    ~TextureLibrary() = delete;

    TextureLibrary(const TextureLibrary &other) = delete;
    TextureLibrary(TextureLibrary &&other)      = delete;

    TextureLibrary &operator=(const TextureLibrary &other) = delete;
    TextureLibrary &operator=(TextureLibrary &&other)      = delete;

private:
    static std::unordered_map<std::string, Texture2D *> _textures;
};

} // namespace btx

#endif // BRASSTACKS_ASSETLIBRARIES_TEXTURELIBRARY_HPP