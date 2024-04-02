#include "brasstacks/brasstacks.hpp"
#include "brasstacks/assets/libraries/TextureLibrary.hpp"

namespace btx {

TextureLibrary::TextureList TextureLibrary::_loaded_textures { };

// =============================================================================
void TextureLibrary::shutdown() {
    _loaded_textures.clear();
}

// =============================================================================
void TextureLibrary::unload_texture(TextureIter const iter) {
    _loaded_textures.erase(iter);
}

} // namespace btx