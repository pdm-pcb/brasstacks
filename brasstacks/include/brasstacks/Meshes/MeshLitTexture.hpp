#ifndef BRASSTACKS_MESHES_MESHLITTEXTURE_HPP
#define BRASSTACKS_MESHES_MESHLITTEXTURE_HPP

#include "brasstacks/Meshes/Mesh.hpp"
#include "brasstacks/Textures/Texture2D.hpp"

#include <bitset>

namespace btx {

class MeshLitTexture : public Mesh {
public:
    typedef struct {
        glm::vec4 position;
        glm::vec4 normal;
        glm::vec4 tangent;
        glm::vec4 bitangent;
        glm::vec2 texcoord;
    } Vertex;

    void bind_vertex_buffer() const override;

    std::size_t index_count() const override { return _face_count * 3; }

    void set_texture(const char *diffuse_filepath,
                     const char *normal_filepath,
                     bool flip_vertical, bool gen_mipmaps,
                     const Texture2D::MinFilter min_filter,
                     const Texture2D::MagFilter mag_filter,
                     const Texture2D::Wrap wrap_s,
                     const Texture2D::Wrap wrap_t);

    MeshLitTexture(const Primitives primitive,
                    const float u_repeat     = 1.0f,
                    const float v_repeat     = 1.0f,
                    const float scale        = 1.0f,
                    const float plane_offset = 0.0f);
    ~MeshLitTexture();

    MeshLitTexture() = delete;

    MeshLitTexture(const MeshLitTexture&) = delete;
    MeshLitTexture(MeshLitTexture&&)      = delete;
    MeshLitTexture& operator=(const MeshLitTexture&) = delete;
    MeshLitTexture& operator=(MeshLitTexture&&)      = delete;

private:
    VertexBuffer *_buffer;
    Vertex       *_vertices;
    Face         *_faces;

    Texture2D *_diffuse;
    Texture2D *_normal;

    std::size_t _vertex_count;
    std::size_t _face_count;

    void _build_cube(const float scale);
    void _build_xzplane(const float scale, const float y_offset,
                        const float u_repeat, const float v_repeat);
    void _build_xyplane(const float scale, const float z_offset,
                        const float u_repeat, const float v_repeat);

    void _calc_tbn();
};

} // namespace btx

#endif // BRASSTACKS_MESHES_MESHLITTEXTURE_HPP