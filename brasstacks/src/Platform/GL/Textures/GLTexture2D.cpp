#include "brasstacks/System/pch.hpp"
#include "brasstacks/Platform/GL/Textures/GLTexture2D.hpp"

#include "brasstacks/Engine/RenderContext.hpp"
#include "brasstacks/Platform/GL/GLContextWGL.hpp"

#include <stb/stb_image.h>

namespace btx {

void GLTexture2D::bind(const uint32_t slot) {
    _slot = slot;
    glBindTextureUnit(_slot, _handle);
}

void GLTexture2D::set_data(const void *data, const GLint width,
                           const GLint height, const std::size_t channels) {

    GLenum format;
    GLenum internal_format;
    _set_channels(channels, format, internal_format);

    glPixelStorei(GL_PACK_ALIGNMENT, static_cast<GLint>(channels));
    glPixelStorei(GL_UNPACK_ALIGNMENT, static_cast<GLint>(channels));

    glTextureStorage2D(_handle, 1, format, width, height);
    glTextureSubImage2D(
        _handle,
        0, 0, 0,
        width, height,
        internal_format,
        GL_UNSIGNED_BYTE,
        data
    );

    if(_has_mipmaps) {
	    glGenerateTextureMipmap(_handle);
    }
}

void GLTexture2D::_set_params(const MinFilter min_filter,
                              const MagFilter mag_filter,
                              const Wrap wrap_s, const Wrap wrap_t) {
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
}


void GLTexture2D::_set_channels(const std::size_t channels, GLenum &format,
                                GLenum &internal_format) {
    switch(channels) {
        case 1:
            format = GL_R8;
            internal_format = GL_RED;
            break;
        case 2:
            format = GL_RG8;
            internal_format = GL_RG;
            break;
        case 3:
            format = GL_RGB8;
            internal_format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA8;
            internal_format = GL_RGBA;
            break;
        default:
            BTX_ENGINE_WARN("Unknown channel count for new GLTexture2D");
            format = GL_INVALID_ENUM;
            internal_format = GL_INVALID_ENUM;
            assert(false);
    }
}

GLTexture2D::GLTexture2D(const char *filepath, const bool flip_vertical,
                         const bool gen_mipmaps,
                         const MinFilter min_filter,
                         const MagFilter mag_filter,
                         const Wrap wrap_s, const Wrap wrap_t) :
	_handle      { GL_NONE },
    _slot        { 0 },
    _has_mipmaps { gen_mipmaps }
{
	static_assert(GL_TEXTURE1 - GL_TEXTURE0 == 1);

    int width, height, channels;
	stbi_set_flip_vertically_on_load(static_cast<int>(flip_vertical));
	uint8_t *buffer = stbi_load(
        filepath,
        &width,
        &height,
		&channels,
        0
    );

	if(buffer == nullptr) {
		BTX_ENGINE_WARN("Failed to load image {}\n\t"
                        "Size/Channels: {}x{}@{}\n\t"
                        "Error: '{}'",
                        filepath, width, height, channels,
						stbi_failure_reason());
        assert(false);
	}

    auto *context = dynamic_cast<GLContextWGL *>(RenderContext::active());
    context->make_current();

    glCreateTextures(GL_TEXTURE_2D, 1, &_handle);
    assert(_handle != GL_NONE);
    _set_params(min_filter, mag_filter, wrap_s, wrap_t);

    GLenum format;
    GLenum internal_format;
    _set_channels(channels, format, internal_format);

    glTextureStorage2D(_handle, 1, format, width, height);
    glTextureSubImage2D(
        _handle,
        0, 0, 0,
        width, height,
        internal_format,
        GL_UNSIGNED_BYTE,
        buffer
    );

	if(buffer != nullptr) {
		stbi_image_free(buffer);
		buffer = nullptr;
	}

    if(gen_mipmaps) {
	    glGenerateTextureMipmap(_handle);
    }
    
    context->release_context();
}

GLTexture2D::GLTexture2D(const bool gen_mipmaps,
                         const MinFilter min_filter,
                         const MagFilter mag_filter,
                         const Wrap wrap_s, const Wrap wrap_t) :
	_handle      { GL_NONE },
    _slot        { 0 },
    _has_mipmaps { gen_mipmaps }
{
	static_assert(GL_TEXTURE1 - GL_TEXTURE0 == 1);

    glCreateTextures(GL_TEXTURE_2D, 1, &_handle);
    assert(_handle != GL_NONE);
    _set_params(min_filter, mag_filter, wrap_s, wrap_t);
}

GLTexture2D::~GLTexture2D() {
	glDeleteTextures(1, &_handle);
}

} // namespace btx