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

ScreenLog::Atlas ScreenLog::_mono { };
ScreenLog::Atlas ScreenLog::_sans { };

MeshScreenLog *ScreenLog::_mesh   = nullptr;
Shader        *ScreenLog::_shader = nullptr;

void ScreenLog::write_line(const char *text, uint32_t x_pos, uint32_t y_pos,
                           float scale)
{
    auto camera_id = CameraBag::get_active();
    auto camera    = ECS::get_active()->get<cCamera>(camera_id);
    _shader->bind();
    _shader->update_camera(camera->view_matrix, camera->ortho_proj);

    _mesh->bind_vertex_buffer();
    _mesh->bind_texture();

    float x = static_cast<float>(x_pos);
    float y = static_cast<float>(y_pos);

    for(std::size_t index = 0; index < strlen(text); index++) {
        char c = text[index];
        GlyphLoc ch = _mono.map[c];

        float w = static_cast<float>(ch.bottom_right.x - ch.top_left.x) * scale;
        float h = static_cast<float>(ch.bottom_right.y - ch.top_left.y) * scale;

        float vertices[4][6] = {
            { x,     y + h, 0.0f, 1.0f, ch.top_left.x,     ch.top_left.y     },
            { x,     y,     0.0f, 1.0f, ch.top_left.x,     ch.bottom_right.y },
            { x + w, y,     0.0f, 1.0f, ch.bottom_right.x, ch.bottom_right.y },
            { x + w, y + h, 0.0f, 1.0f, ch.bottom_right.x, ch.top_left.y     },
        };
        _mesh->update_buffer(vertices, sizeof(vertices));

        // RenderConfig::enable_blending();

        ::glDrawElements(
            GL_TRIANGLES,
            static_cast<GLsizei>(_mesh->index_count()),
            GL_UNSIGNED_INT, 0
        );

        // RenderConfig::disable_blending();

        x += w;
    }
}

void ScreenLog::init() {
    ::FT_Library library = nullptr;
    ::FT_Face    mono    = nullptr;
    ::FT_Face    sans    = nullptr;

    ::FT_Error result = ::FT_Init_FreeType(&library);
    if(result != ::FT_Err_Ok) {
        BTX_ENGINE_WARN("Could not initialize FreeType");
        assert(false);
    }

    _load("../../assets/fonts/Hack.ttf",     library, mono, _mono);
    _load("../../assets/fonts/OpenSans.ttf", library, sans, _sans);

    ::FT_Done_Face(mono);
    ::FT_Done_Face(sans);
    ::FT_Done_FreeType(library);

    _shader = ShaderLibrary::checkout("screen_log");
    _mesh = dynamic_cast<MeshScreenLog *>(
        MeshLibrary::checkout("screen_log")
    );
    _mesh->set_texture(_mono.handle);
}

void ScreenLog::shutdown() {
    delete _mono.handle;
    delete _sans.handle;
}

void ScreenLog::_load(const char *filepath, FT_LibraryRec_ *library,
                      FT_FaceRec_ *face, Atlas &atlas)
{
    ::FT_Error result = ::FT_Err_Ok;

    result = ::FT_New_Face(library, filepath, 0, &face);
    if(result != ::FT_Err_Ok) {
        BTX_ENGINE_WARN("FreeType could not load font {}", filepath);
        assert(false);
    }

    ::FT_Set_Char_Size(face, 0, 12 * 64, 300u, 300u);

    _get_atlas_dimensions(face, atlas);
    BTX_ENGINE_TRACE("Font atlas {} will be {}x{}", filepath,
                     atlas.dimensions.x, atlas.dimensions.y);
    _build_atlas(face, atlas);
}

void ScreenLog::_get_atlas_dimensions(FT_FaceRec_ *face, Atlas &atlas) {
    int row_length  = 0;
    int row_height  = 0;
    int max_ascent  = 0;
    int max_descent = 0;
    int max_width   = 0;

    FT_GlyphSlot glyph = face->glyph;
    for(unsigned int c = MIN_CHAR; c < MAX_CHAR; c++) {
        if(FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;

        int h_adv = glyph->linearHoriAdvance / 65536;
        int v_adv = glyph->linearVertAdvance / 65536;

        row_length += h_adv;

        if(h_adv > max_width) {
            max_width = h_adv;
        }

        if(v_adv > row_height) {
            row_height = v_adv;
        }

        if(glyph->bitmap_top > max_ascent) {
            max_ascent = glyph->bitmap_top;
        }

        if((glyph->metrics.height >> 6) - glyph->bitmap_top > max_descent) {
            max_descent = (glyph->metrics.height >> 6) - glyph->bitmap_top;
        }
        
        BTX_ENGINE_TRACE("{:c}: {}x{}", c, h_adv, v_adv);
    }

    row_height = std::max(row_height, max_ascent + max_descent);

    int buffer_size = row_length * row_height;
    float buff_sqrt = std::abs(std::sqrtf(static_cast<float>(buffer_size)));

    int image_width  = static_cast<int>(std::ceil(buff_sqrt / max_width)  * max_width);
    int image_height = static_cast<int>(std::ceil(buff_sqrt / row_height) * row_height);

    atlas.dimensions = { image_width, image_height };
    atlas.row_height = row_height;
    atlas.max_ascent = max_ascent;
    atlas.max_descent = max_descent;
}


void ScreenLog::_build_atlas(FT_FaceRec_ *face, Atlas &atlas) {
    int x_offset = 0;
    int y_offset = 0;
    FT_GlyphSlot glyph = face->glyph;

    int buffer_size = atlas.dimensions.x * atlas.dimensions.y;
    unsigned char *buffer = new unsigned char[buffer_size];
    memset(buffer, '\0', buffer_size);

    for(int c = MIN_CHAR; c < MAX_CHAR; c++) {
        if(FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;

        int x_bearing = glyph->metrics.horiBearingX / 64;
        int x_advance = glyph->linearHoriAdvance / 65536;

        if(x_offset + x_advance > atlas.dimensions.x) {
            x_offset = 0;
            y_offset += atlas.row_height;
        }

        if(x_bearing < 0 && x_offset == 0) {
            x_offset += -x_bearing;
        }

        x_offset += x_bearing;
        int y_origin_offset = atlas.max_ascent - glyph->bitmap_top;

        uint32_t glyph_index = 0;
        uint32_t atlas_index = 0;

        for(uint32_t x = 0; x < glyph->bitmap.width; x++) {
            for(uint32_t y = 0; y < glyph->bitmap.rows; y++) {
                glyph_index = x + y * glyph->bitmap.width;
                atlas_index = (x + x_offset) +
                              (y + y_offset + y_origin_offset)
                              * atlas.dimensions.x;

                buffer[atlas_index] = glyph->bitmap.buffer[glyph_index];
            }
        }

        x_offset += x_advance - x_bearing;

        atlas.map[c].top_left     = { x_offset - x_advance, y_offset };
        atlas.map[c].bottom_right = { x_offset, y_offset + atlas.row_height };
    }

    atlas.handle = Texture2D::create(
        false,
        Texture2D::MinFilter::linear,
        Texture2D::MagFilter::linear,
        Texture2D::Wrap::clamp_to_edge,
        Texture2D::Wrap::clamp_to_edge
    );

    atlas.handle->set_data(
        buffer,
        atlas.dimensions.x,
        atlas.dimensions.y,
        1
    );

    delete[] buffer;
}

} // namespace btx