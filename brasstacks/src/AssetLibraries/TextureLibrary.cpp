#include "brasstacks/System/pch.hpp"
#include "brasstacks/AssetLibraries/TextureLibrary.hpp"

namespace btx {

std::unordered_map<std::string, Texture2D *> TextureLibrary::_textures;

void TextureLibrary::load(const char *path, const char *key,
                          const bool flip_vertical,
                          const bool gen_mipmaps,
                          const Texture2D::MinFilter min_filter,
                          const Texture2D::MagFilter mag_filter,
                          const Texture2D::Wrap wrap_s,
                          const Texture2D::Wrap wrap_t)
{
    if(_textures.find(key) != _textures.end()) {
        BTX_ENGINE_WARN("Texture already exists");
        assert(false);
        return;
    }

    Texture2D *texture = Texture2D::create(
        path, flip_vertical, gen_mipmaps,
        min_filter, mag_filter, wrap_s, wrap_t
    );

    _textures.insert({ key, texture });
}

Texture2D * TextureLibrary::checkout(const char *key) {
    auto texture = _textures.find(key);
    if(texture == _textures.end()) {
        BTX_ENGINE_WARN("Cannot find texture");
        assert(false);
        return nullptr;
    }

    return texture->second;
}

void TextureLibrary::shutdown() {
    for(auto &texture : _textures) {
        delete texture.second;
    }

    _textures.clear();
}

void TextureLibrary::remove(const char *key) {
    auto texture = _textures.find(key);
    if(texture == _textures.end()) {
        BTX_ENGINE_WARN("Cannot find texture '{}'", key);
        assert(false);
        return;
    }

    delete texture->second;

    _textures.erase(texture);
}

} // namespace btx