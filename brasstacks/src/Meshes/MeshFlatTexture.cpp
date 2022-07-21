#include "brasstacks/System/pch.hpp"
#include "brasstacks/Meshes/MeshFlatTexture.hpp"

#include "brasstacks/Meshes/VertexBuffer.hpp"

namespace btx {

void MeshFlatTexture::bind_vertex_buffer() const {
    _buffer->bind();
}

void MeshFlatTexture::_build_cube(const float scale) {
    _vertices = new Vertex[_vertex_count] {
        // front face
        {{ -0.5f * scale, -0.5f * scale,  0.5f * scale, 1.0f }, { 0.0f, 0.0f }},
        {{  0.5f * scale, -0.5f * scale,  0.5f * scale, 1.0f }, { 1.0f, 0.0f }},
        {{  0.5f * scale,  0.5f * scale,  0.5f * scale, 1.0f }, { 1.0f, 1.0f }},
        {{ -0.5f * scale,  0.5f * scale,  0.5f * scale, 1.0f }, { 0.0f, 1.0f }},

        // back face
        {{ -0.5f * scale, -0.5f * scale, -0.5f * scale, 1.0f }, { 0.0f, 0.0f }},
        {{  0.5f * scale, -0.5f * scale, -0.5f * scale, 1.0f }, { 1.0f, 0.0f }},
        {{  0.5f * scale,  0.5f * scale, -0.5f * scale, 1.0f }, { 1.0f, 1.0f }},
        {{ -0.5f * scale,  0.5f * scale, -0.5f * scale, 1.0f }, { 0.0f, 1.0f }},

        // left face
        {{ -0.5f * scale, -0.5f * scale, -0.5f * scale, 1.0f }, { 1.0f, 0.0f }},
        {{ -0.5f * scale, -0.5f * scale,  0.5f * scale, 1.0f }, { 1.0f, 1.0f }},
        {{ -0.5f * scale,  0.5f * scale,  0.5f * scale, 1.0f }, { 0.0f, 1.0f }},
        {{ -0.5f * scale,  0.5f * scale, -0.5f * scale, 1.0f }, { 0.0f, 0.0f }},

        // right face
        {{  0.5f * scale, -0.5f * scale,  0.5f * scale, 1.0f }, { 0.0f, 0.0f }},
        {{  0.5f * scale, -0.5f * scale, -0.5f * scale, 1.0f }, { 0.0f, 1.0f }},
        {{  0.5f * scale,  0.5f * scale, -0.5f * scale, 1.0f }, { 1.0f, 1.0f }},
        {{  0.5f * scale,  0.5f * scale,  0.5f * scale, 1.0f }, { 1.0f, 0.0f }},

        // bottom face
        {{  0.5f * scale, -0.5f * scale,  0.5f * scale, 1.0f }, { 0.0f, 0.0f }},
        {{ -0.5f * scale, -0.5f * scale,  0.5f * scale, 1.0f }, { 1.0f, 0.0f }},
        {{ -0.5f * scale, -0.5f * scale, -0.5f * scale, 1.0f }, { 1.0f, 1.0f }},
        {{  0.5f * scale, -0.5f * scale, -0.5f * scale, 1.0f }, { 0.0f, 1.0f }},

        // top face
        {{ -0.5f * scale,  0.5f * scale,  0.5f * scale, 1.0f }, { 0.0f, 0.0f }},
        {{  0.5f * scale,  0.5f * scale,  0.5f * scale, 1.0f }, { 1.0f, 0.0f }},
        {{  0.5f * scale,  0.5f * scale, -0.5f * scale, 1.0f }, { 1.0f, 1.0f }},
        {{ -0.5f * scale,  0.5f * scale, -0.5f * scale, 1.0f }, { 0.0f, 1.0f }},
    };

    _faces = new Mesh::Face[_face_count] {
		{  0,  1,  2 }, {  2,  3,  0 }, {  6,  5,  4 }, {  4,  7,  6 },
		{  8,  9, 10 }, { 10, 11,  8 }, { 12, 13, 14 }, { 14, 15, 12 },
		{ 16, 17, 18 }, { 18, 19, 16 }, { 20, 21, 22 }, { 22, 23, 20 }
    };
}

void MeshFlatTexture::_build_xzplane(const float scale, const float y_offset) {
    _vertices = new Vertex[_vertex_count] {
        {{ -0.5f * scale,  y_offset,  0.5f * scale, 1.0f }, { 0.0f, 0.0f }},
        {{  0.5f * scale,  y_offset,  0.5f * scale, 1.0f }, { 1.0f, 0.0f }},
        {{  0.5f * scale,  y_offset, -0.5f * scale, 1.0f }, { 1.0f, 1.0f }},
        {{ -0.5f * scale,  y_offset, -0.5f * scale, 1.0f }, { 0.0f, 1.0f }}
    };

    _faces = new Face[_face_count] {
        { 0, 1, 2 },
        { 2, 3, 0 }
    };
}

void MeshFlatTexture::_build_xyplane(const float scale, const float z_offset) {
    _vertices = new Vertex[_vertex_count] {
        {{ -0.5f * scale, -0.5f * scale,  z_offset, 1.0f }, { 0.0f, 0.0f }},
        {{  0.5f * scale, -0.5f * scale,  z_offset, 1.0f }, { 1.0f, 0.0f }},
        {{  0.5f * scale,  0.5f * scale,  z_offset, 1.0f }, { 1.0f, 1.0f }},
        {{ -0.5f * scale,  0.5f * scale,  z_offset, 1.0f }, { 0.0f, 1.0f }}
    };

    _faces = new Face[_face_count] {
        { 0, 1, 2 },
        { 2, 3, 0 }
    };
}

MeshFlatTexture::MeshFlatTexture(const Primitives primitive,
                                 const float scale,
                                 const float plane_offset) :
    _buffer { VertexBuffer::create({
        { "position", VBElement::Type::vec4f },
        { "texcoord", VBElement::Type::vec2f }
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
            _vertex_count = PLANE_VERTS;
            _face_count = PLANE_FACES;
            _build_xzplane(scale, plane_offset);
            break;

        case Primitives::XYPlane:
            _vertex_count = PLANE_VERTS;
            _face_count = PLANE_FACES;
            _build_xyplane(scale, plane_offset);
            break;
    }

    _buffer->set_buffer(_vertices, _vertex_count * sizeof(Vertex));
    _buffer->set_indices(_faces, _face_count);
}

MeshFlatTexture::~MeshFlatTexture() {
    delete _buffer;
    delete _vertices;
    delete _faces;
}

} // namespace btx