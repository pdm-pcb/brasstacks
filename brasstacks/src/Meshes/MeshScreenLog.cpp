#include "brasstacks/System/pch.hpp"
#include "brasstacks/Meshes/MeshScreenLog.hpp"

#include "brasstacks/Meshes/VertexBuffer.hpp"
#include "brasstacks/Textures/Texture2D.hpp"

namespace btx {

void MeshScreenLog::bind_vertex_buffer() const {
    _buffer->bind();
}

void MeshScreenLog::bind_texture() const {
    _diffuse->bind();
}

void MeshScreenLog::set_buffer(const void *data, const std::size_t size) {
    _buffer->set_buffer(data, size);
}

MeshScreenLog::MeshScreenLog() :
    _buffer { VertexBuffer::create({
        { "position", VBElement::Type::vec4f }
    })},
    _vertices     { },
    _faces        { },
    _diffuse      { nullptr },
    _vertex_count { QUAD_VERTS },
    _face_count   { QUAD_FACES }
{
    _buffer = VertexBuffer::create({
        { "position", VBElement::Type::vec4f },
    });

    _buffer->set_indices(_faces, QUAD_FACES);
}

MeshScreenLog::~MeshScreenLog() {
    delete _buffer;
}

} // namespace btx