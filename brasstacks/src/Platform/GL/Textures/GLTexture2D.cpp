#include "brasstacks/System/pch.hpp"
#include "brasstacks/Platform/GL/Textures/GLTexture2D.hpp"

#include <stb/stb_image.h>

namespace btx {

void GLTexture2D::bind(const uint32_t slot) {
    _slot = slot;
	glActiveTexture(GL_TEXTURE0 + _slot);
	glBindTexture(GL_TEXTURE_2D, _handle);
}

void GLTexture2D::unbind() const {
	glActiveTexture(GL_TEXTURE0 + _slot);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLTexture2D::GLTexture2D(const char *filepath, const bool inverted,
                         const bool gen_mipmaps) :
	_handle { 0 },
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

	glGenTextures(1, &_handle);
	bind();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLenum format = GL_NONE;
    if(bytes_per_pixel == 3) {
        format = GL_RGB;
    }
    else if(bytes_per_pixel == 4) {
        format = GL_RGBA;
    }

	glTexImage2D(
        GL_TEXTURE_2D, 0,
        format,
        static_cast<GLsizei>(width),
        static_cast<GLsizei>(height),
        0, format,
        GL_UNSIGNED_BYTE, buffer
    );

    if(gen_mipmaps) {
	    glGenerateMipmap(GL_TEXTURE_2D);
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