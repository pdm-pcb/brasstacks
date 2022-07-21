#include "brasstacks/System/pch.hpp"
#include "brasstacks/Meshes/MeshFlatColor.hpp"

#include "brasstacks/Meshes/VertexBuffer.hpp"

namespace btx {

void MeshFlatColor::bind_vertex_buffer() const {
    _buffer->bind();
}

void MeshFlatColor::_build_cube(const float scale) {
    _vertices = new Vertex[_vertex_count] {
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

    _faces = new Mesh::Face[_face_count] {
		{  0,  1,  2 }, {  2,  3,  0 }, {  6,  5,  4 }, {  4,  7,  6 },
		{  8,  9, 10 }, { 10, 11,  8 }, { 12, 13, 14 }, { 14, 15, 12 },
		{ 16, 17, 18 }, { 18, 19, 16 }, { 20, 21, 22 }, { 22, 23, 20 }
    };
}

void MeshFlatColor::_build_xz_plane(const float scale) {
    _vertices = new Vertex[_vertex_count] {
        {{ -0.5f * scale, -0.5f * scale,  0.0f, 1.0f },
         { 0.5f, 0.0f, 0.0f, 1.0f }},
        {{  0.5f * scale, -0.5f * scale,  0.0f, 1.0f },
         { 0.0f, 0.5f, 0.0f, 1.0f }},
        {{  0.5f * scale,  0.5f * scale,  0.0f, 1.0f },
         { 0.0f, 0.0f, 0.5f, 1.0f }},
        {{ -0.5f * scale,  0.5f * scale,  0.0f, 1.0f },
         { 0.5f, 0.5f, 0.0f, 1.0f }},
    };

    _faces = new Mesh::Face[_face_count] {
		{ 0, 1, 2 },
        { 2, 3, 0 }
    };
}

MeshFlatColor::MeshFlatColor(const Primitives primitive, const float scale) :
    _buffer { VertexBuffer::create({
        { "POSITION", VBElement::Type::vec4f },
        { "COLOR",    VBElement::Type::vec4f },
    })},
    _vertices { nullptr },
    _faces    { nullptr },
    _vertex_count { 0 },
    _face_count   { 0 }
{
    switch(primitive) {
        case Primitives::Cube:
            _vertex_count = CUBE_VERTS;
            _face_count = CUBE_FACES;
            _build_cube(scale);
            break;

        case Primitives::XZPlane:
            _build_xz_plane(scale);
            _vertex_count = PLANE_VERTS;
            _face_count = PLANE_FACES;
            break;
    }

    _buffer->set_buffer(_vertices, _vertex_count * sizeof(Vertex));
    _buffer->set_indices(_faces, _face_count);
}

MeshFlatColor::~MeshFlatColor() {
    delete _buffer;
    delete _vertices;
    delete _faces;
}

} // namespace btx