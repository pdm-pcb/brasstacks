#ifndef BRASSTACKS_MESHES_MESHFLATCOLOR_HPP
#define BRASSTACKS_MESHES_MESHFLATCOLOR_HPP

#include "brasstacks/Meshes/Mesh.hpp"

namespace btx {

class MeshFlatColor : public Mesh {
public:
    typedef struct {
        glm::vec4 position;
        glm::vec4 color;
    } Vertex;

    void bind_vertex_buffer() const override;
    std::size_t index_count() const override { return _face_count * 3; }

    explicit MeshFlatColor(const Primitives primitive,
                           const glm::vec3 &color,
                           const float scale = 1.0f,
                           const float plane_offset = 0.0f);

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

    void _build_cube(const glm::vec3 &color, const float scale);
    void _build_xzplane(const glm::vec3 &color,
                        const float scale, const float y_offset);
    void _build_xyplane(const glm::vec3 &color,
                        const float scale, const float z_offset);
};

} // namespace btx

#endif // BRASSTACKS_MESHES_MESHFLATCOLOR_HPP