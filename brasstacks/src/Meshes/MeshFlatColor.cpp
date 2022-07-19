#include "brasstacks/System/pch.hpp"
#include "brasstacks/Meshes/MeshFlatColor.hpp"

#include "brasstacks/Meshes/VertexBuffer.hpp"

namespace btx {

void MeshFlatColor::_build_cube(Vertex **vertices, Face **faces,
                                const float scale) {
    *vertices = new Vertex[CUBE_VERTS] {
        // front face
        {{ -0.5f * scale, -0.5f * scale,  0.5f * scale, 1.0f },
         { 0.1f, 0.2f, 0.65f, 1.0f }},
        {{  0.5f * scale, -0.5f * scale,  0.5f * scale, 1.0f },
         { 0.1f, 0.2f, 0.65f, 1.0f }},
        {{  0.5f * scale,  0.5f * scale,  0.5f * scale, 1.0f },
         { 0.1f, 0.2f, 0.65f, 1.0f }},
        {{ -0.5f * scale,  0.5f * scale,  0.5f * scale, 1.0f },
         { 0.1f, 0.2f, 0.65f, 1.0f }},

        // back face
        {{ -0.5f * scale, -0.5f * scale, -0.5f * scale, 1.0f },
         { 0.65f, 0.1f, 0.2f, 1.0f }},
        {{  0.5f * scale, -0.5f * scale, -0.5f * scale, 1.0f },
         { 0.65f, 0.1f, 0.2f, 1.0f }},
        {{  0.5f * scale,  0.5f * scale, -0.5f * scale, 1.0f },
         { 0.65f, 0.1f, 0.2f, 1.0f }},
        {{ -0.5f * scale,  0.5f * scale, -0.5f * scale, 1.0f },
         { 0.65f, 0.1f, 0.2f, 1.0f }},

        // left face
        {{ -0.5f * scale, -0.5f * scale, -0.5f * scale, 1.0f },
         { 0.2f, 0.65f, 0.1f, 1.0f }},
        {{ -0.5f * scale, -0.5f * scale,  0.5f * scale, 1.0f },
         { 0.2f, 0.65f, 0.1f, 1.0f }},
        {{ -0.5f * scale,  0.5f * scale,  0.5f * scale, 1.0f },
         { 0.2f, 0.65f, 0.1f, 1.0f }},
        {{ -0.5f * scale,  0.5f * scale, -0.5f * scale, 1.0f },
         { 0.2f, 0.65f, 0.1f, 1.0f }},

        // right face
        {{  0.5f * scale, -0.5f * scale,  0.5f * scale, 1.0f },
         { 0.65f, 0.3f, 0.4f, 1.0f }},
        {{  0.5f * scale, -0.5f * scale, -0.5f * scale, 1.0f },
         { 0.65f, 0.3f, 0.4f, 1.0f }},
        {{  0.5f * scale,  0.5f * scale, -0.5f * scale, 1.0f },
         { 0.65f, 0.3f, 0.4f, 1.0f }},
        {{  0.5f * scale,  0.5f * scale,  0.5f * scale, 1.0f },
         { 0.65f, 0.3f, 0.4f, 1.0f }},

        // bottom face
        {{  0.5f * scale, -0.5f * scale,  0.5f * scale, 1.0f },
         { 0.4f, 0.65f, 0.3f, 1.0f }},
        {{ -0.5f * scale, -0.5f * scale,  0.5f * scale, 1.0f },
         { 0.4f, 0.65f, 0.3f, 1.0f }},
        {{ -0.5f * scale, -0.5f * scale, -0.5f * scale, 1.0f },
         { 0.4f, 0.65f, 0.3f, 1.0f }},
        {{  0.5f * scale, -0.5f * scale, -0.5f * scale, 1.0f },
         { 0.4f, 0.65f, 0.3f, 1.0f }},

        // top face
        {{ -0.5f * scale,  0.5f * scale,  0.5f * scale, 1.0f },
         { 0.3f, 0.4f, 0.65f, 1.0f }},
        {{  0.5f * scale,  0.5f * scale,  0.5f * scale, 1.0f },
         { 0.3f, 0.4f, 0.65f, 1.0f }},
        {{  0.5f * scale,  0.5f * scale, -0.5f * scale, 1.0f },
         { 0.3f, 0.4f, 0.65f, 1.0f }},
        {{ -0.5f * scale,  0.5f * scale, -0.5f * scale, 1.0f },
         { 0.3f, 0.4f, 0.65f, 1.0f }},
    };

    *faces = new Mesh::Face[CUBE_FACES] {
		{  0,  1,  2 }, {  2,  3,  0 }, {  6,  5,  4 }, {  4,  7,  6 },
		{  8,  9, 10 }, { 10, 11,  8 }, { 12, 13, 14 }, { 14, 15, 12 },
		{ 16, 17, 18 }, { 18, 19, 16 }, { 20, 21, 22 }, { 22, 23, 20 }
    };
}

void MeshFlatColor::_build_xz_plane(Vertex **vertices, Face **faces,
                                    const float scale) {
    *vertices = new Vertex[PLANE_VERTS] {
        {{ -0.5f * scale, -0.5f * scale,  0.0f, 1.0f },
         { 0.5f, 0.0f, 0.0f, 1.0f }},
        {{  0.5f * scale, -0.5f * scale,  0.0f, 1.0f },
         { 0.0f, 0.5f, 0.0f, 1.0f }},
        {{  0.5f * scale,  0.5f * scale,  0.0f, 1.0f },
         { 0.0f, 0.0f, 0.5f, 1.0f }},
        {{ -0.5f * scale,  0.5f * scale,  0.0f, 1.0f },
         { 0.5f, 0.5f, 0.0f, 1.0f }},
    };

    *faces = new Mesh::Face[PLANE_FACES] {
		{ 0, 1, 2 },
        { 2, 3, 0 }
    };
}

void MeshFlatColor::create(const Primitives primitive, VertexBuffer **vb,
                           Vertex **vertices, Face **faces, const float scale)
{
    *vb = VertexBuffer::create({
        { "POSITION", VBElement::Type::vec4f },
        { "COLOR",    VBElement::Type::vec4f },
    });

    switch(primitive) {
        case Primitives::Cube:
            _build_cube(vertices, faces, scale);
            (*vb)->set_buffer(*vertices, CUBE_VERTS * sizeof(Vertex));
            (*vb)->set_indices(*faces, CUBE_FACES);
            break;

        case Primitives::XZPlane:
            _build_xz_plane(vertices, faces, scale);
            (*vb)->set_buffer(*vertices, PLANE_VERTS * sizeof(Vertex));
            (*vb)->set_indices(*faces, PLANE_FACES);
            break;
    }
}

} // namespace btx