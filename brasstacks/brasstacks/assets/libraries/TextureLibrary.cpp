#include "brasstacks/brasstacks.hpp"
#include "brasstacks/assets/libraries/TextureLibrary.hpp"

namespace btx {

TextureLibrary::TextureList TextureLibrary::_loaded_textures { };

// =============================================================================
void TextureLibrary::shutdown() {
    for(auto *texture : _loaded_textures) {
        delete texture;
    }
    _loaded_textures.clear();
}

// =============================================================================
TextureLibrary::TextureIter const
TextureLibrary::load_texture(std::string_view const filepath)
{
    _loaded_textures.emplace_back(new Texture(filepath));
    return --_loaded_textures.end();
}

// =============================================================================
void TextureLibrary::unload_texture(TextureIter const iter) {
    _loaded_textures.erase(iter);
}

// =============================================================================
void TextureLibrary::update_samplers() {
    Renderer::wait_device_idle();
    for(auto &texture : _loaded_textures) {
        texture->update_sampler();
        texture->update_descriptor_set();
    }
}

} // namespace btx