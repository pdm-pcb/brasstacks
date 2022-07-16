#ifndef BRASSTACKS_MESHES_MESHFLATCOLOR_HPP
#define BRASSTACKS_MESHES_MESHFLATCOLOR_HPP

#include "brasstacks/Meshes/Mesh.hpp"

namespace btx {

class VertexBuffer;

class MeshFlatColor : public Mesh {
public:
    struct Vertex {
        glm::vec4 position;
        glm::vec4 color;
    };

    void bind() const override;

    uint32_t index_count() const override;

    explicit MeshFlatColor(Primitives primitive);
    ~MeshFlatColor();

    MeshFlatColor() = delete;

    MeshFlatColor(const MeshFlatColor&) = delete;
    MeshFlatColor(MeshFlatColor&&)      = delete;
    MeshFlatColor& operator=(const MeshFlatColor&) = delete;
    MeshFlatColor& operator=(MeshFlatColor&&)      = delete;

private:
    Vertex  *_vertices;
    uint32_t _vertex_count;

    Face     *_faces;
    uint32_t _face_count;

    VertexBuffer *_vb;

    void _build_cube(const float scale);
    void _build_xz_plane(const float scale);
};

} // namespace btx

#endif // BRASSTACKS_MESHES_MESHFLATCOLOR_HPP