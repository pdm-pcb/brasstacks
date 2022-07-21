#ifndef BRASSTACKS_MESHES_MESHFLATCOLOR_HPP
#define BRASSTACKS_MESHES_MESHFLATCOLOR_HPP

#include "brasstacks/Meshes/Mesh.hpp"
#include "brasstacks/Meshes/VertexLayout.hpp"

namespace btx {

class VertexBuffer;

class MeshFlatColor : public Mesh {
public:
    typedef struct {
        glm::vec4 position;
        glm::vec4 color;
    } Vertex;

    void bind_vertex_buffer() const override;
    std::size_t index_count() const override { return _face_count * 3; }

    explicit MeshFlatColor(const Primitives primitive,
                           const float scale = 1.0f);
    ~MeshFlatColor();

    MeshFlatColor() = delete;

    MeshFlatColor(const MeshFlatColor&) = delete;
    MeshFlatColor(MeshFlatColor&&)      = delete;
    MeshFlatColor& operator=(const MeshFlatColor&) = delete;
    MeshFlatColor& operator=(MeshFlatColor&&)      = delete;

private:
    VertexBuffer *_buffer;
    Vertex       *_vertices;
    Face         *_faces;

    std::size_t _vertex_count;
    std::size_t _face_count;

    void _build_cube(const float scale);
    void _build_xz_plane(const float scale);
};

} // namespace btx

#endif // BRASSTACKS_MESHES_MESHFLATCOLOR_HPP