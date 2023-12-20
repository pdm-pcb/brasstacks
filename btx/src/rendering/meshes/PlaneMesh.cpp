#include "brasstacks/rendering/meshes/PlaneMesh.hpp"

#include "brasstacks/math/math.hpp"

namespace btx {

PlaneMesh::PlaneMesh(vkDevice const &device, float const scale,
                     std::array<math::Vec3, 4> const &color) :
    Mesh(device)
{
    Mesh::_set_vertices({
        {{ -scale, -scale, 0.0f }, color[0] },
        {{ -scale,  scale, 0.0f }, color[1] },
        {{  scale,  scale, 0.0f }, color[2] },
        {{  scale, -scale, 0.0f }, color[3] },

    });

    Mesh::_set_indices({ 0, 1, 2,  0, 2, 3 });
}

} // namespace btx