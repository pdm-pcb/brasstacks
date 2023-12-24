#include "brasstacks/rendering/meshes/PlaneMesh.hpp"

#include "brasstacks/math/math.hpp"

namespace btx {

PlaneMesh::PlaneMesh(vkDevice const &device, float const scale,
                     std::array<math::Vec3, 4> const &color) :
    Mesh(device)
{
    Mesh::_set_vertices({
        {{ -scale, -scale, 0.0f }, color[0], { 0.0f, 0.0f } },
        {{ -scale,  scale, 0.0f }, color[1], { 0.0f, 1.0f } },
        {{  scale,  scale, 0.0f }, color[2], { 1.0f, 1.0f } },
        {{  scale, -scale, 0.0f }, color[3], { 1.0f, 0.0f } },
    });

    Mesh::_set_indices({ 0, 1, 2,  0, 2, 3 });
}

} // namespace btx