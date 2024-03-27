#ifndef BRASSTACKS_ASSETS_MESHES_PLANEMESH_HPP
#define BRASSTACKS_ASSETS_MESHES_PLANEMESH_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/assets/meshes/Mesh.hpp"
#include "brasstacks/math/colors.hpp"

namespace btx {

class vkDevice;

class PlaneMesh final : public Mesh {
public:
    PlaneMesh(vkDevice const &device,
              std::array<Color, 4> const &corner_colors,
              float const scale = 1.0f);

    ~PlaneMesh() override = default;

    PlaneMesh() = delete;

    PlaneMesh(PlaneMesh &&) = delete;
    PlaneMesh(PlaneMesh const &) = delete;

    PlaneMesh & operator=(PlaneMesh &&) = delete;
    PlaneMesh & operator=(PlaneMesh const &) = delete;
};

} // namespace btx

#endif // BRASSTACKS_ASSETS_MESHES_PLANEMESH_HPP