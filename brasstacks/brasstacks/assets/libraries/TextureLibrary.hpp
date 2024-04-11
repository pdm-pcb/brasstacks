#ifndef BRASSTACKS_ASSETS_LIBRARIES_TEXTURELIBRARY_HPP
#define BRASSTACKS_ASSETS_LIBRARIES_TEXTURELIBRARY_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/assets/textures/Texture.hpp"

namespace btx {

class TextureLibrary final {
public:
    using TextureList = std::list<Texture *>;
    using TextureIter = TextureList::iterator;

    static void init() { }
    static void shutdown();

    static TextureIter const load_texture(std::string_view const filepath);

    static void unload_texture(TextureIter const iter);

    static void update_samplers();

    TextureLibrary() = delete;
    ~TextureLibrary() = delete;

    TextureLibrary(TextureLibrary &&) = delete;
    TextureLibrary(TextureLibrary const &) = delete;

    TextureLibrary & operator=(TextureLibrary &&) = delete;
    TextureLibrary & operator=(TextureLibrary const &) = delete;

private:
    static TextureList _loaded_textures;
};

} // namespace btx

#endif // BRASSTACKS_ASSETS_LIBRARIES_TEXTURELIBRARY_HPP