#ifndef BRASSTACKS_RENDERING_MESHES_PLANEMESH_HPP
#define BRASSTACKS_RENDERING_MESHES_PLANEMESH_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/rendering/meshes/Mesh.hpp"

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
};

} // namespace btx

#endif // BRASSTACKS_RENDERING_MESHES_PLANEMESH_HPP