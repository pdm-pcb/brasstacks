#include "brasstacks/assets/meshes/CubeMesh.hpp"

#include "brasstacks/math/math.hpp"

namespace btx {

CubeMesh::CubeMesh(float const scale,
                   std::span<Color const, 8> const corner_colors) :
    Mesh { }
{
    Mesh::_set_vertices(
        std::span<Vertex const, 24> {{
            // front face
            {{ -scale, -scale,  scale }, corner_colors[1], { 0.0f, 0.0f }}, // 0
            {{ -scale,  scale,  scale }, corner_colors[1], { 0.0f, 1.0f }}, // 1
            {{  scale,  scale,  scale }, corner_colors[2], { 1.0f, 1.0f }}, // 2
            {{  scale, -scale,  scale }, corner_colors[3], { 1.0f, 0.0f }}, // 3

            // back face
            {{  scale, -scale, -scale }, corner_colors[4], { 0.0f, 0.0f }}, // 4
            {{  scale,  scale, -scale }, corner_colors[5], { 0.0f, 1.0f }}, // 5
            {{ -scale,  scale, -scale }, corner_colors[6], { 1.0f, 1.0f }}, // 6
            {{ -scale, -scale, -scale }, corner_colors[7], { 1.0f, 0.0f }}, // 7

            // top face
            {{ -scale,  scale,  scale }, corner_colors[1], { 0.0f, 0.0f }},
            {{ -scale,  scale, -scale }, corner_colors[6], { 0.0f, 1.0f }},
            {{  scale,  scale, -scale }, corner_colors[5], { 1.0f, 1.0f }},
            {{  scale,  scale,  scale }, corner_colors[2], { 1.0f, 0.0f }},

            // bottom face
            {{ -scale, -scale, -scale }, corner_colors[7], { 0.0f, 0.0f }},
            {{ -scale, -scale,  scale }, corner_colors[0], { 0.0f, 1.0f }},
            {{  scale, -scale,  scale }, corner_colors[3], { 1.0f, 1.0f }},
            {{  scale, -scale, -scale }, corner_colors[4], { 1.0f, 0.0f }},

            // left face
            {{ -scale, -scale, -scale }, corner_colors[7], { 0.0f, 0.0f }},
            {{ -scale,  scale, -scale }, corner_colors[6], { 0.0f, 1.0f }},
            {{ -scale,  scale,  scale }, corner_colors[1], { 1.0f, 1.0f }},
            {{ -scale, -scale,  scale }, corner_colors[0], { 1.0f, 0.0f }},

            // right face
            {{  scale, -scale,  scale }, corner_colors[3], { 0.0f, 0.0f }},
            {{  scale,  scale,  scale }, corner_colors[2], { 0.0f, 1.0f }},
            {{  scale,  scale, -scale }, corner_colors[5], { 1.0f, 1.0f }},
            {{  scale, -scale, -scale }, corner_colors[4], { 1.0f, 0.0f }},
        }}
    );

    Mesh::_set_indices(
        std::span<Index const, 36> {{
            // front face
            0, 1, 2,
            0, 2, 3,

            // back face
            4, 5, 6,
            4, 6, 7,

            // top face
            8,  9, 10,
            8, 10, 11,

            // bottom face
            12, 13, 14,
            12, 14, 15,

            // left face
            16, 17, 18,
            16, 18, 19,

            // right face
            20, 21, 22,
            20, 22, 23,
        }}
    );
}

} // namespace btx