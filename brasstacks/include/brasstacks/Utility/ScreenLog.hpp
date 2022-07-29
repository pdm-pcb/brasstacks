#ifndef BRASSTACKS_UTILITY_SCREENLOG_HPP
#define BRASSTACKS_UTILITY_SCREENLOG_HPP

#include "brasstacks/Meshes/Mesh.hpp"
#include "brasstacks/System/glm.hpp"

#include <array>;

struct FT_LibraryRec_;
struct FT_FaceRec_;
struct FT_GlyphSlotRec_;

namespace btx {

constexpr std::size_t MIN_CHAR = 0;
constexpr std::size_t MAX_CHAR = 256;
constexpr std::size_t FONT_SIZE = 24;

class Texture2D;
class MeshScreenLog;
class MeshFlatTexture;
class Shader;

class ScreenLog {
public:
    struct GlyphLoc {
        glm::ivec2  top_left;
        glm::ivec2  bottom_right;
    };
    using CharMap = std::array<GlyphLoc, MAX_CHAR - MIN_CHAR>;

    struct Atlas {
        glm::ivec2 dimensions;
        uint32_t row_height;
        uint32_t max_ascent;
        uint32_t max_descent;

        CharMap map;
        Texture2D *handle;
    };

    static void add_line(const char *text);
    static void draw();

    static void init();
    static void shutdown();

private:
    static Atlas _mono;
    static Atlas _sans;

    static MeshScreenLog *_mesh;
    static Shader        *_shader;

    static std::array<char, 128> _first_line_buffer;

    static void _load(const char *filepath, FT_LibraryRec_ *library,
                      FT_FaceRec_ *face, Atlas &atlas);

    static void _get_atlas_dimensions(FT_FaceRec_ *face, Atlas &atlas);
    static void _build_atlas(FT_FaceRec_ *face, Atlas &atlas);
};

} // namespace btx

#endif // BRASSTACKS_UTILITY_SCREENLOG_HPP