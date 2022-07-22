#include "brasstacks/System/pch.hpp"
#include "brasstacks/Platform/GL/Textures/GLTexture2D.hpp"

#include <stb/stb_image.h>

namespace btx {

void GLTexture2D::bind(const uint32_t slot) {
    _slot = slot;
    glBindTextureUnit(_slot, _handle);
}

void GLTexture2D::unbind() const {
	glActiveTexture(GL_TEXTURE0 + _slot);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLTexture2D::GLTexture2D(const char *filepath, const bool inverted,
                         const bool gen_mipmaps) :
	_handle { GL_INVALID_ENUM },
    _slot   { 0 }
{
	static_assert(GL_TEXTURE1 - GL_TEXTURE0 == 1);

    int width, height, bytes_per_pixel;

	stbi_set_flip_vertically_on_load(static_cast<int>(inverted));
	uint8_t *buffer = stbi_load(
        filepath,
        &width,
        &height,
		&bytes_per_pixel,
        0
    );

	if(buffer == nullptr) {
		BTX_ENGINE_WARN("Failed to load image {}\n\t"
                        "Size/Depth: {}x{}@{}\n\t"
                        "Error: '{}'",
                        filepath, width, height, bytes_per_pixel,
						stbi_failure_reason());
        assert(false);
	}

    glCreateTextures(GL_TEXTURE_2D, 1, &_handle);

    assert(_handle != GL_INVALID_ENUM);

	glTextureParameteri(_handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(_handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(_handle, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(_handle, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLenum format = GL_INVALID_ENUM;
    GLenum internal_format = GL_INVALID_ENUM;
    if(bytes_per_pixel == 3) {
        format = GL_RGB8;
        internal_format = GL_RGB;
    }
    else if(bytes_per_pixel == 4) {
        format = GL_RGBA8;
        internal_format = GL_RGBA;
    }
    else {
        assert(false);
    }

    glTextureStorage2D(_handle, 1, format, width, height);
    glTextureSubImage2D(
        _handle,
        0, 0, 0,
        width, height,
        internal_format,
        GL_UNSIGNED_BYTE,
        buffer
    );

    if(gen_mipmaps) {
	    glGenerateTextureMipmap(_handle);
    }
	unbind();

	if(buffer != nullptr) {
		stbi_image_free(buffer);
		buffer = nullptr;
	}
	BTX_ENGINE_TRACE("Loaded texture {}", filepath);
}

GLTexture2D::~GLTexture2D() {
	glDeleteTextures(1, &_handle);
}

} // namespace btx