#ifndef BRASSTACKS_UTILITY_SCREENLOG_HPP
#define BRASSTACKS_UTILITY_SCREENLOG_HPP

#include "brasstacks/Meshes/Mesh.hpp"
#include "brasstacks/System/glm.hpp"

#include <array>;

struct FT_FaceRec_;
struct FT_LibraryRec_;

namespace btx {

constexpr unsigned long int ASCII_OFFSET  = 32;
constexpr unsigned long int CHARMAP_COUNT = 128 - ASCII_OFFSET;

class Texture2D;
class MeshScreenLog;
class MeshFlatTexture;
class Shader;

class ScreenLog {
public:
    struct Glyph {
        Texture2D  *handle;
        glm::ivec2  dimensions;
        glm::ivec2  bearing;
        long int    advance;
    };
    using CharMap = std::array<Glyph, CHARMAP_COUNT>;

    typedef struct {
        glm::vec4 position;
    } Vertex;

    static void write_line(const char *text, float x, float y, float scale);

    static void init();
    static void shutdown();

private:
    static CharMap _mono_map;
    static CharMap _sans_map;

    static MeshScreenLog *_mesh;
    static Shader        *_shader;

    static void _load(const char *filepath,
                      FT_LibraryRec_ *library,
                      FT_FaceRec_ *face,
                      CharMap &map);
};

} // namespace btx

#endif // BRASSTACKS_UTILITY_SCREENLOG_HPP