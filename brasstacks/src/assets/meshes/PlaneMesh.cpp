#include "brasstacks/assets/meshes/PlaneMesh.hpp"

#include "brasstacks/tools/ConsoleLog.hpp"
#include "brasstacks/math/math.hpp"

namespace btx {

PlaneMesh::PlaneMesh(vkDevice const &device,
                     std::span<Color const, 4> const &corner_colors,
                     float const scale) :
    Mesh(device)
{
    Mesh::_set_vertices(
        std::span<Vertex const, 4> {{
            {{ -scale, -scale, 0.0f }, corner_colors[0], { 0.0f, 0.0f }},
            {{ -scale,  scale, 0.0f }, corner_colors[1], { 0.0f, 1.0f }},
            {{  scale,  scale, 0.0f }, corner_colors[2], { 1.0f, 1.0f }},
            {{  scale, -scale, 0.0f }, corner_colors[3], { 1.0f, 0.0f }},
        }}
    );

    Mesh::_set_indices(std::span<Index const, 6> {{ 0, 1, 2,  0, 2, 3 }});
}

} // namespace btx