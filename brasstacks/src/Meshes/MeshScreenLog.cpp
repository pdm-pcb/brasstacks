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
    _buffer { VertexBuffer::create({
            { "position", VBElement::Type::vec4f },
            { "texcoord", VBElement::Type::vec2f },
    })},
    _diffuse      { nullptr },
    _vertex_count { 6 },
    _face_count   { 2 }
{
    _buffer->set_buffer(nullptr, sizeof(Vertex) * _vertex_count);
}

MeshScreenLog::~MeshScreenLog() {
    delete _buffer;
}

} // namespace btx