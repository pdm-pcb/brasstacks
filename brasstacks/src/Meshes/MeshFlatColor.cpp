#include "brasstacks/System/pch.hpp"
#include "brasstacks/Meshes/MeshFlatColor.hpp"

#include "brasstacks/Meshes/VertexBuffer.hpp"

namespace btx {

void MeshFlatColor::bind() const {
    _vb->bind();
}

uint32_t MeshFlatColor::index_count() const {
    return 3u * _face_count;
}

void MeshFlatColor::_build_cube(const float scale) {
    _vertices = new Vertex[_vertex_count] {
        // front face
        {{ -0.5f * scale, -0.5f * scale,  0.5f * scale, 1.0f },
         { 0.0f, 0.0f, 1.0f, 1.0f }},
        {{  0.5f * scale, -0.5f * scale,  0.5f * scale, 1.0f },
         { 0.0f, 0.0f, 1.0f, 1.0f }},
        {{  0.5f * scale,  0.5f * scale,  0.5f * scale, 1.0f },
         { 0.0f, 0.0f, 1.0f, 1.0f }},
        {{ -0.5f * scale,  0.5f * scale,  0.5f * scale, 1.0f },
         { 0.0f, 0.0f, 1.0f, 1.0f }},

        // back face
        {{ -0.5f * scale, -0.5f * scale, -0.5f * scale, 1.0f },
         { 0.0f, 0.0f, 1.0f, 1.0f }},
        {{  0.5f * scale, -0.5f * scale, -0.5f * scale, 1.0f },
         { 0.0f, 0.0f, 1.0f, 1.0f }},
        {{  0.5f * scale,  0.5f * scale, -0.5f * scale, 1.0f },
         { 0.0f, 0.0f, 1.0f, 1.0f }},
        {{ -0.5f * scale,  0.5f * scale, -0.5f * scale, 1.0f },
         { 0.0f, 0.0f, 1.0f, 1.0f }},

        // left face
        {{ -0.5f * scale, -0.5f * scale, -0.5f * scale, 1.0f },
         { 1.0f, 0.0f, 0.0f, 1.0f }},
        {{ -0.5f * scale, -0.5f * scale,  0.5f * scale, 1.0f },
         { 1.0f, 0.0f, 0.0f, 1.0f }},
        {{ -0.5f * scale,  0.5f * scale,  0.5f * scale, 1.0f },
         { 1.0f, 0.0f, 0.0f, 1.0f }},
        {{ -0.5f * scale,  0.5f * scale, -0.5f * scale, 1.0f },
         { 1.0f, 0.0f, 0.0f, 1.0f }},

        // right face
        {{  0.5f * scale, -0.5f * scale,  0.5f * scale, 1.0f },
         { 1.0f, 0.0f, 0.0f, 1.0f }},
        {{  0.5f * scale, -0.5f * scale, -0.5f * scale, 1.0f },
         { 1.0f, 0.0f, 0.0f, 1.0f }},
        {{  0.5f * scale,  0.5f * scale, -0.5f * scale, 1.0f },
         { 1.0f, 0.0f, 0.0f, 1.0f }},
        {{  0.5f * scale,  0.5f * scale,  0.5f * scale, 1.0f },
         { 1.0f, 0.0f, 0.0f, 1.0f }},

        // bottom face
        {{  0.5f * scale, -0.5f * scale,  0.5f * scale, 1.0f },
         { 0.0f, 0.0f, 1.0f, 1.0f }},
        {{ -0.5f * scale, -0.5f * scale,  0.5f * scale, 1.0f },
         { 0.0f, 0.0f, 1.0f, 1.0f }},
        {{ -0.5f * scale, -0.5f * scale, -0.5f * scale, 1.0f },
         { 0.0f, 0.0f, 1.0f, 1.0f }},
        {{  0.5f * scale, -0.5f * scale, -0.5f * scale, 1.0f },
         { 0.0f, 0.0f, 1.0f, 1.0f }},

        // top face
        {{ -0.5f * scale,  0.5f * scale,  0.5f * scale, 1.0f },
         { 0.0f, 0.0f, 1.0f, 1.0f }},
        {{  0.5f * scale,  0.5f * scale,  0.5f * scale, 1.0f },
         { 0.0f, 0.0f, 1.0f, 1.0f }},
        {{  0.5f * scale,  0.5f * scale, -0.5f * scale, 1.0f },
         { 0.0f, 0.0f, 1.0f, 1.0f }},
        {{ -0.5f * scale,  0.5f * scale, -0.5f * scale, 1.0f },
         { 0.0f, 0.0f, 1.0f, 1.0f }},
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

MeshFlatColor::MeshFlatColor(Primitives primitive) :
    _vertices     { nullptr },
    _vertex_count { 0u },
    _faces        { nullptr },
    _face_count   { 0u },
    _vb           { nullptr }
{
    switch(primitive) {
        case Primitives::Cube:
            _vertex_count = 24;
            _face_count   = 12;
            _build_cube(1.0f);
            break;

        case Primitives::XZPlane:
            _vertex_count = 4;
            _face_count   = 2;
            _build_xz_plane(1.0f);
            break;
    }

    _vb = VertexBuffer::create({
        { "POSITION", VBElement::Type::vec4f },
        { "COLOR",    VBElement::Type::vec4f },
    });

    _vb->set_buffer(_vertices, _vertex_count * sizeof(Vertex));
    _vb->set_indices(_faces, _face_count);
}

MeshFlatColor::~MeshFlatColor() {
    delete[] _vertices;
    delete[] _faces;

    delete _vb;
}

} // namespace btx