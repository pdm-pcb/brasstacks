#ifndef BRASSTACKS_MESHES_MESHFLATTEXTURE_HPP
#define BRASSTACKS_MESHES_MESHFLATTEXTURE_HPP

#include "brasstacks/Meshes/Mesh.hpp"

namespace btx {

class Texture2D;

class MeshFlatTexture : public Mesh {
public:
    typedef struct {
        glm::vec4 position;
        glm::vec2 texcoord;
    } Vertex;

    void bind_vertex_buffer() const override;
    std::size_t index_count() const override { return _face_count * 3; }

    MeshFlatTexture(const Primitives primitive,
                    const char *diffuse_filepath,
                    const bool invert_texture,
                    const float u_repeat,
                    const float v_repeat,
                    const bool gen_mipmaps,
                    const float scale = 1.0f,
                    const float plane_offset = 0.0f);
    ~MeshFlatTexture();

    MeshFlatTexture() = delete;

    MeshFlatTexture(const MeshFlatTexture&) = delete;
    MeshFlatTexture(MeshFlatTexture&&)      = delete;
    MeshFlatTexture& operator=(const MeshFlatTexture&) = delete;
    MeshFlatTexture& operator=(MeshFlatTexture&&)      = delete;

private:
    VertexBuffer *_buffer;
    Vertex       *_vertices;
    Face         *_faces;

    Texture2D *_diffuse;

    std::size_t _vertex_count;
    std::size_t _face_count;

    void _build_cube(const float scale);
    void _build_xzplane(const float scale, const float y_offset,
                        const float u_repeat, const float v_repeat);
    void _build_xyplane(const float scale, const float z_offset,
                        const float u_repeat, const float v_repeat);
};

} // namespace btx

#endif // BRASSTACKS_MESHES_MESHFLATTEXTURE_HPP