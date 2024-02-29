#ifndef BRASSTACKS_RENDERING_MESHES_CUBEMESH_HPP
#define BRASSTACKS_RENDERING_MESHES_CUBEMESH_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/rendering/meshes/Mesh.hpp"

namespace btx {

class vkDevice;

class CubeMesh final : public Mesh {
public:
    CubeMesh(vkDevice const &device,
             float const scale = 1.0f,
             std::array<math::Vec3, 8> const &corner_colors = {
                 colors::white, colors::white, colors::white, colors::white,
                 colors::white, colors::white, colors::white, colors::white,
            });

    ~CubeMesh() = default;

    CubeMesh() = delete;

    CubeMesh(CubeMesh &&) = delete;
    CubeMesh(CubeMesh const &) = delete;

    CubeMesh & operator=(CubeMesh &&) = delete;
    CubeMesh & operator=(CubeMesh const &) = delete;
};

} // namespace btx

#endif // BRASSTACKS_RENDERING_MESHES_CUBEMESH_HPP