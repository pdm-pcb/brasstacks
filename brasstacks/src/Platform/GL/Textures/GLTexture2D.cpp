#include "brasstacks/System/pch.hpp"
#include "brasstacks/Platform/GL/Textures/GLTexture2D.hpp"

#include <stb/stb_image.h>

namespace btx {

void GLTexture2D::bind(const uint32_t slot) {
    _slot = slot;
    glBindTextureUnit(_slot, _handle);
}

GLTexture2D::GLTexture2D(const char *filepath, const bool flip_vertical,
                         const bool gen_mipmaps,
                         const MinFilter min_filter,
                         const MagFilter mag_filter,
                         const Wrap wrap_s, const Wrap wrap_t) :
	_handle { GL_NONE },
    _slot   { 0 }
{
	static_assert(GL_TEXTURE1 - GL_TEXTURE0 == 1);

    int width, height, bytes_per_pixel;
	stbi_set_flip_vertically_on_load(static_cast<int>(flip_vertical));
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
    assert(_handle != GL_NONE);

    GLint param = 0;
    switch(min_filter) {
        case MinFilter::nearest:
            param = GL_NEAREST;
            break;
        case MinFilter::linear:
            param = GL_LINEAR;
            break;
        case MinFilter::neatest_mipmap_nearest:
            param = GL_NEAREST_MIPMAP_NEAREST;
            break;
        case MinFilter::linear_mipmap_nearest:
            param = GL_LINEAR_MIPMAP_NEAREST;
            break;
        case MinFilter::nearest_mipmap_linear:
            param = GL_NEAREST_MIPMAP_LINEAR;
            break;
        case MinFilter::linear_mipmap_lienar:
            param = GL_LINEAR_MIPMAP_LINEAR;
            break;
        default: assert(false);
    }
	glTextureParameteri(_handle, GL_TEXTURE_MIN_FILTER, param);


    switch(mag_filter) {
        case MagFilter::nearest:
            param = GL_NEAREST;
            break;
        case MagFilter::linear:
            param = GL_LINEAR;
            break;
        default: assert(false);
    }
	glTextureParameteri(_handle, GL_TEXTURE_MAG_FILTER, param);

    switch(wrap_s) {
        case Wrap::clamp_to_edge:        param = GL_CLAMP_TO_EDGE;        break;
        case Wrap::clamp_to_border:      param = GL_CLAMP_TO_BORDER;      break;
        case Wrap::mirror_repeat:        param = GL_MIRRORED_REPEAT;      break;
        case Wrap::repeat:               param = GL_REPEAT;               break;
        case Wrap::mirror_clamp_to_edge: param = GL_MIRROR_CLAMP_TO_EDGE; break;
    }
	glTextureParameteri(_handle, GL_TEXTURE_WRAP_S, param);

    switch(wrap_t) {
        case Wrap::clamp_to_edge:        param = GL_CLAMP_TO_EDGE;        break;
        case Wrap::clamp_to_border:      param = GL_CLAMP_TO_BORDER;      break;
        case Wrap::mirror_repeat:        param = GL_MIRRORED_REPEAT;      break;
        case Wrap::repeat:               param = GL_REPEAT;               break;
        case Wrap::mirror_clamp_to_edge: param = GL_MIRROR_CLAMP_TO_EDGE; break;
    }
	glTextureParameteri(_handle, GL_TEXTURE_WRAP_T, param);

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