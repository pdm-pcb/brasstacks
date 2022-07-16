#ifndef BRASSTACKS_MESHES_MESH_HPP
#define BRASSTACKS_MESHES_MESH_HPP

#include <cstdint>

namespace btx {

class Mesh {
public:
    struct Face {
        uint32_t index_a;
        uint32_t index_b;
        uint32_t index_c;
    };

    enum class Primitives {
        Cube, Sphere, Skybox, XZPlane, XYPlane
    };

    virtual void bind() const = 0;
    virtual uint32_t index_count() const = 0;

    virtual ~Mesh() = default;

    Mesh(const Mesh &&) = delete;
    Mesh(Mesh &)        = delete;

    Mesh & operator=(const Mesh &&) = delete;
    Mesh & operator=(Mesh &)        = delete;

protected:
    Mesh()          = default;
};

} // namespace btx

#endif // BRASSTACKS_MESHES_MESH_HPP