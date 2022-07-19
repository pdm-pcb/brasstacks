#ifndef BRASSTACKS_MESHES_MESHFLATCOLOR_HPP
#define BRASSTACKS_MESHES_MESHFLATCOLOR_HPP

#include "brasstacks/Meshes/Mesh.hpp"

namespace btx {

class VertexBuffer;

class MeshFlatColor : public Mesh {
public:
    typedef struct {
        glm::vec4 position;
        glm::vec4 color;
    } Vertex;

    static void create(const Primitives primitive, VertexBuffer **vb,
                       Vertex **vertices, Face **faces, const float scale);

    MeshFlatColor()  = delete;
    ~MeshFlatColor() = default;

    MeshFlatColor(const MeshFlatColor&) = delete;
    MeshFlatColor(MeshFlatColor&&)      = delete;
    MeshFlatColor& operator=(const MeshFlatColor&) = delete;
    MeshFlatColor& operator=(MeshFlatColor&&)      = delete;

private:
    static void _build_cube(Vertex **vertices, Face **faces, const float scale);
    static void _build_xz_plane(Vertex **vertices, Face **faces,
                                const float scale);
};

} // namespace btx

#endif // BRASSTACKS_MESHES_MESHFLATCOLOR_HPP