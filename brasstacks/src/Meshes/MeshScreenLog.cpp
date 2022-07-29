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

void MeshScreenLog::update_buffer(const void *data, const std::size_t size,
                                  const std::size_t offset) {
    _buffer->update_buffer(data, size, offset);
}

void MeshScreenLog::set_buffer(const void *data, const std::size_t size) {
    _buffer->set_buffer(data, size);
}

MeshScreenLog::MeshScreenLog() :
    _buffer       { nullptr },
    _diffuse      { nullptr },
    _vertices     { nullptr },
    _faces        { nullptr },
    _vertex_count { QUAD_VERTS },
    _face_count   { QUAD_FACES }
{
    _vertices = new Vertex[_vertex_count];
    _faces    = new Face[_face_count] {
        { 0, 1, 2},
        { 0, 2, 3}
    };

    _buffer = VertexBuffer::create({
        { "position", VBElement::Type::vec4f },
        { "texcoord", VBElement::Type::vec2f },
    });

    _buffer->set_buffer(nullptr, sizeof(Vertex) * _vertex_count);
    _buffer->set_indices(_faces, _face_count * 3);
}

MeshScreenLog::~MeshScreenLog() {
    delete _buffer;
    delete _vertices;
    delete _faces;
}

} // namespace btx