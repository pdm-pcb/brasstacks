#include "brasstacks/assets/meshes/CubeMesh.hpp"

#include "brasstacks/math/math.hpp"

namespace btx {

CubeMesh::CubeMesh(vkDevice const &device,
                   std::array<Color, 8> const &corner_colors,
                   float const scale) :
    Mesh(device)
{
    Mesh::_set_vertices({
        {{ -scale, -scale,  scale }, corner_colors[0]},
        {{ -scale,  scale,  scale }, corner_colors[1]},
        {{  scale,  scale,  scale }, corner_colors[2]},
        {{  scale, -scale,  scale }, corner_colors[3]},

        {{  scale, -scale, -scale }, corner_colors[4]},
        {{  scale,  scale, -scale }, corner_colors[5]},
        {{ -scale,  scale, -scale }, corner_colors[6]},
        {{ -scale, -scale, -scale }, corner_colors[7]},
    });

    Mesh::_set_indices({
        // front face
        0, 1, 2,
        0, 2, 3,

        // back face
        4, 5, 6,
        4, 6, 7,

        // top face
        1, 6, 5,
        1, 5, 2,

        // bottom face
        7, 0, 3,
        7, 3, 4,

        // left face
        7, 6, 1,
        7, 1, 0,

        // right face
        3, 2, 5,
        3, 5, 4,
    });
}

} // namespace btx