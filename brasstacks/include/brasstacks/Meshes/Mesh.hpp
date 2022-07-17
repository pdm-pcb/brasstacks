#ifndef BRASSTACKS_MESHES_MESH_HPP
#define BRASSTACKS_MESHES_MESH_HPP

#include <cstdint>

namespace btx {

class Mesh {
public:
    struct Face {
        std::uint32_t index_a;
        std::uint32_t index_b;
        std::uint32_t index_c;
    };

    enum class Primitives {
        Cube, Sphere, Skybox, XZPlane, XYPlane
    };

    virtual void bind() const = 0;
    virtual std::uint32_t index_count() const = 0;

    void set_world_mat(const glm::mat4 &world_mat) { _world_mat = world_mat; }
    const glm::mat4 & world_mat() const { return _world_mat; }

    virtual ~Mesh() = default;

    Mesh(const Mesh &&) = delete;
    Mesh(Mesh &)        = delete;

    Mesh & operator=(const Mesh &&) = delete;
    Mesh & operator=(Mesh &)        = delete;

protected:
    Mesh() : _world_mat { 1.0f } { }

private:
    glm::mat4 _world_mat;
};

} // namespace btx

#endif // BRASSTACKS_MESHES_MESH_HPP