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
              float const scale = 1.0f,
              std::array<math::Vec3, 4> const &color =
                  { colors::white, colors::white, colors::white, colors::white }
             );

    ~PlaneMesh() = default;

    PlaneMesh() = delete;

    PlaneMesh(PlaneMesh &&) = delete;
    PlaneMesh(PlaneMesh const &) = delete;

    PlaneMesh & operator=(PlaneMesh &&) = delete;
    PlaneMesh & operator=(PlaneMesh const &) = delete;
};

} // namespace btx

#endif // BRASSTACKS_ASSETS_MESHES_PLANEMESH_HPP