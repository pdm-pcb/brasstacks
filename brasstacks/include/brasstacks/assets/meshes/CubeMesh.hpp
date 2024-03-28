#ifndef BRASSTACKS_ASSETS_MESHES_CUBEMESH_HPP
#define BRASSTACKS_ASSETS_MESHES_CUBEMESH_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/assets/meshes/Mesh.hpp"

namespace btx {

class vkDevice;

class CubeMesh final : public Mesh {
public:
    CubeMesh(vkDevice const &device,
             std::span<Color const, 8> const corner_colors,
             float const scale = 1.0f);

    ~CubeMesh() override = default;

    CubeMesh() = delete;

    CubeMesh(CubeMesh &&) = delete;
    CubeMesh(CubeMesh const &) = delete;

    CubeMesh & operator=(CubeMesh &&) = delete;
    CubeMesh & operator=(CubeMesh const &) = delete;
};

} // namespace btx

#endif // BRASSTACKS_ASSETS_MESHES_CUBEMESH_HPP