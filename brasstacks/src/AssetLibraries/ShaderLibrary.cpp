#include "brasstacks/System/pch.hpp"
#include "brasstacks/AssetLibraries/ShaderLibrary.hpp"

namespace btx {

std::unordered_map<std::string, Shader *> ShaderLibrary::_shaders;

void ShaderLibrary::load(const char *vertex_filepath,
                         const char *pixel_filepath,
                         const char *key) {
    if(_shaders.find(key) != _shaders.end()) {
        BTX_ENGINE_WARN("Shader '{}' already exists", key);
        assert(false);
        return;
    }
    assert(false);
    // TODO: a smarter way to handle this perhaps?
}

Shader * ShaderLibrary::checkout(const char *key) {
    auto texture = _shaders.find(key);
    if(texture == _shaders.end()) {
        BTX_ENGINE_WARN("Cannot find shader '{}'", key);
        assert(false);
        return nullptr;
    }

    return texture->second;
}

void ShaderLibrary::init() {
    auto flat_color   = new ShaderFlatColor();
    auto flat_texture = new ShaderFlatTexture();
    auto lit_texture  = new ShaderLitTexture();

    _shaders.insert({ "flat_color",   flat_color   });
    _shaders.insert({ "flat_texture", flat_texture });
    _shaders.insert({ "lit_texture",  lit_texture  });
}

void ShaderLibrary::shutdown() {
    for(auto &texture : _shaders) {
        delete texture.second;
    }

    _shaders.clear();
}

void ShaderLibrary::remove(const char *key) {
    auto texture = _shaders.find(key);
    if(texture == _shaders.end()) {
        BTX_ENGINE_WARN("Cannot find shader '{}'", key);
        assert(false);
        return;
    }

    delete texture->second;

    _shaders.erase(texture);
}

} // namespace btx