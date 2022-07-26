#include "brasstacks/System/pch.hpp"
#include "brasstacks/Utility/ScreenLog.hpp"

#include "brasstacks/Textures/Texture2D.hpp"
#include "brasstacks/Meshes/VertexBuffer.hpp"
#include "brasstacks/AssetLibraries/ShaderLibrary.hpp"
#include "brasstacks/AssetLibraries/MeshLibrary.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>

namespace btx {

ScreenLog::CharMap ScreenLog::_mono_map { };
ScreenLog::CharMap ScreenLog::_sans_map { };

MeshScreenLog *ScreenLog::_mesh   = nullptr;
Shader        *ScreenLog::_shader = nullptr;

void ScreenLog::write_line(std::string text, float x, float y, float scale) {
    _shader->bind();
    _shader->update_camera(
        glm::mat4(1.0f),
        glm::ortho(
            -RenderConfig::aspect_ratio,
            RenderConfig::aspect_ratio
            -1.0f, 1.0f
            -1.0f, 1.0f
        )
    );

    for(std::string::const_iterator c = text.begin(); c != text.end(); c++) {
        Glyph ch = _mono_map[*c - ASCII_OFFSET];

        if(ch.handle == nullptr) {
            x += (ch.advance >> 6) * scale;
            continue;
        }

        float xpos = x + ch.bearing.x * scale;
        float ypos = y - (ch.dimensions.y - ch.dimensions.y) * scale;

        float w = ch.dimensions.x * scale;
        float h = ch.dimensions.y * scale;

        _mesh->set_texture(ch.handle);
        _mesh->bind_texture();

        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos,     ypos,       0.0f, 1.0f },
        };
        _mesh->set_buffer(vertices, sizeof(vertices));

        // RenderConfig::enable_blending();

        ::glDrawElements(
            GL_TRIANGLES,
            static_cast<GLsizei>(_mesh->index_count()),
            GL_UNSIGNED_INT, 0
        );

        // RenderConfig::disable_blending();
    
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
}

void ScreenLog::init() {
    ::FT_Error result = ::FT_Err_Ok;

    ::FT_Library library = nullptr;
    ::FT_Face    mono    = nullptr;
    ::FT_Face    sans    = nullptr;

    result = ::FT_Init_FreeType(&library);
    if(result != ::FT_Err_Ok) {
        BTX_ENGINE_WARN("Could not initialize FreeType");
        assert(false);
    }

    _load("../../assets/fonts/Hack.ttf",     library, mono, _mono_map);
    _load("../../assets/fonts/OpenSans.ttf", library, sans, _sans_map);

    ::FT_Done_Face(mono);
    ::FT_Done_Face(sans);
    ::FT_Done_FreeType(library);

    _shader = ShaderLibrary::checkout("screen_log");
    _mesh   = dynamic_cast<MeshScreenLog *>(
        MeshLibrary::checkout("screen_log")
    );
}

void ScreenLog::shutdown() {
    for(std::size_t index = 0; index < CHARMAP_COUNT; index++) {
        delete _mono_map[index].handle;
        delete _sans_map[index].handle;
    }
}

void ScreenLog::_load(const char *filepath, FT_LibraryRec_ *library,
                      FT_FaceRec_ *face, CharMap &map)
{
    ::FT_Error result = ::FT_Err_Ok;

    result = ::FT_New_Face(library, filepath, 0, &face);
    if(result != ::FT_Err_Ok) {
        BTX_ENGINE_WARN("FreeType could not load font {}", filepath);
        assert(false);
    }

    BTX_ENGINE_TRACE("Loaded font {}", filepath);

    ::FT_Set_Pixel_Sizes(face, 0, 32);

    for(::FT_ULong index = 0; index < CHARMAP_COUNT; ++index) {
        result = ::FT_Load_Char(
            face,
            index + ASCII_OFFSET,
            FT_LOAD_RENDER
        );
        if(result != ::FT_Err_Ok) {
            BTX_ENGINE_WARN("Unable to load '{}' from {}",
                            static_cast<char>(index + ASCII_OFFSET), filepath);
        }

        if(face->glyph->bitmap.buffer == nullptr) {
            BTX_ENGINE_WARN("No bitmap value for character '{}'",
                            static_cast<char>(index));
            continue;
        }

        map[index].handle = Texture2D::create(
            false,
            Texture2D::MinFilter::linear,
            Texture2D::MagFilter::linear,
            Texture2D::Wrap::clamp_to_edge,
            Texture2D::Wrap::clamp_to_edge
        );
        map[index].dimensions.x = face->glyph->bitmap.width;
        map[index].dimensions.y = face->glyph->bitmap.rows;
        map[index].bearing.x    = face->glyph->bitmap_left;
        map[index].bearing.y    = face->glyph->bitmap_top;
        map[index].advance      = face->glyph->advance.x;

        map[index].handle->set_data(face->glyph->bitmap.buffer,
                                    face->glyph->bitmap.width,
                                    face->glyph->bitmap.rows, 1);
    }
    
    BTX_ENGINE_TRACE("Parsed {} font bitmaps", CHARMAP_COUNT - 1);
}

} // namespace btx