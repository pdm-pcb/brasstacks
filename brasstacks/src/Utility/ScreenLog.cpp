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

std::array<char, 128> ScreenLog::_first_line_buffer { '\0' };

void ScreenLog::add_line(const char *text)
{
    float x = 5.0f;
    float y = RenderConfig::window_y_res - FONT_SIZE - 5.0f;

    for(std::size_t index = 0; index < strlen(text); index++) {
        char c = text[index];
        GlyphLoc gl = _mono.map[c];

        float cw = static_cast<float>(gl.bottom_right.x - gl.top_left.x);
        float ch = static_cast<float>(gl.bottom_right.y - gl.top_left.y);

        float aw = static_cast<float>(_mono.dimensions.x);
        float ah = static_cast<float>(_mono.dimensions.y);

        glm::vec2 a_tex = { gl.top_left.x     / aw, gl.top_left.y     / ah };
        glm::vec2 b_tex = { gl.top_left.x     / aw, gl.bottom_right.y / ah };
        glm::vec2 c_tex = { gl.bottom_right.x / aw, gl.bottom_right.y / ah };
        glm::vec2 d_tex = { gl.bottom_right.x / aw, gl.top_left.y     / ah };

        MeshScreenLog::Vertex vertices[4] = {
            {{ x,      y + ch, 0.0f, 1.0f }, {a_tex.x, a_tex.y}},
            {{ x,      y,      0.0f, 1.0f }, {b_tex.x, b_tex.y}},
            {{ x + cw, y,      0.0f, 1.0f }, {c_tex.x, c_tex.y}},
            {{ x + cw, y + ch, 0.0f, 1.0f }, {d_tex.x, d_tex.y}},
        };
        _mesh->add_instance(vertices);

        x += cw;
    }
}

void ScreenLog::draw() {
    snprintf(
        _first_line_buffer.data(), _first_line_buffer.max_size(),
        "Render: %.03fms | Update: %.03fms | Frame time: %.03fms",
        Clock::render_time(), Clock::update_time(), Clock::frame_time()
    );

    add_line(_first_line_buffer.data());

    auto camera_id = CameraBag::get_active();
    auto camera    = ECS::get_active()->get<cCamera>(camera_id);
    _shader->bind();
    _shader->update_camera(camera->view_matrix, camera->ortho_proj);

    RenderConfig::enable_blending();
    _mesh->create_commands();
    RenderConfig::disable_blending();
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

    ::FT_Set_Pixel_Sizes(face, 0, FONT_SIZE);

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