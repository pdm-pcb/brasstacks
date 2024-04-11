#ifndef BRASSTACKS_ASSETS_MESHES_PLANEMESH_HPP
#define BRASSTACKS_ASSETS_MESHES_PLANEMESH_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/assets/meshes/Mesh.hpp"

namespace btx {

class vkDevice;

class PlaneMesh final : public Mesh {
public:
    explicit PlaneMesh(float const scale,
                       std::span<Color const, 4> const corner_colors);

    ~PlaneMesh() = default;

    PlaneMesh() = delete;

    PlaneMesh(PlaneMesh &&) = delete;
    PlaneMesh(PlaneMesh const &) = delete;

    PlaneMesh & operator=(PlaneMesh &&) = delete;
    PlaneMesh & operator=(PlaneMesh const &) = delete;
};

} // namespace btx

#endif // BRASSTACKS_ASSETS_MESHES_PLANEMESH_HPP