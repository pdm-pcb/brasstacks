#ifndef BRASSTACKS_MESHES_MESH_HPP
#define BRASSTACKS_MESHES_MESH_HPP

#include <cstdint>

namespace btx {

class VertexBuffer;

constexpr std::size_t CUBE_VERTS = 24;
constexpr std::size_t CUBE_FACES = 12;
constexpr std::size_t PLANE_VERTS = 4;
constexpr std::size_t PLANE_FACES = 2;

class Mesh {
public:
    typedef struct {
        std::uint32_t index_a;
        std::uint32_t index_b;
        std::uint32_t index_c;
    } Face;

    enum class Primitives {
        Cube, Sphere, Skybox, XZPlane, XYPlane
    };

    enum class Type {
        FlatColor, FlatTexture, LitTexture
    };

    virtual void bind_vertex_buffer() const = 0;
    virtual std::size_t index_count() const = 0;

    Mesh()  = default;
    virtual ~Mesh() = default;

    Mesh(const Mesh &&) = delete;
    Mesh(Mesh &)        = delete;

    Mesh & operator=(const Mesh &&) = delete;
    Mesh & operator=(Mesh &)        = delete;
};

} // namespace btx

#endif // BRASSTACKS_MESHES_MESH_HPP