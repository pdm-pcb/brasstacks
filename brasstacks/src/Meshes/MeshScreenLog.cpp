#include "brasstacks/System/pch.hpp"
#include "brasstacks/Meshes/MeshScreenLog.hpp"

#include "brasstacks/Meshes/VertexBuffer.hpp"
#include "brasstacks/Textures/Texture2D.hpp"

namespace btx {

void MeshScreenLog::add_instance(Vertex vertices[4]) {
    _vertices.emplace_back(Vertex(vertices[0]));
    _vertices.emplace_back(Vertex(vertices[1]));
    _vertices.emplace_back(Vertex(vertices[2]));
    _vertices.emplace_back(Vertex(vertices[3]));
}

void MeshScreenLog::create_commands() {
    uint32_t total_quads = static_cast<uint32_t>(_vertices.size()) / QUAD_VERTS;
    uint32_t sizeof_quad = sizeof(Vertex) * QUAD_VERTS;

    for(std::uint32_t instance = 0; instance < total_quads; ++instance) {
        _commands.emplace_back(MDICommand {
            .index_count = QUAD_INDICES,
            .instance_count = 1,
            .first_index = 0,
            .base_vertex = static_cast<GLuint>(QUAD_INDICES * instance),
            .base_instance = instance,
        });
    }

    _buffer->bind();

    glNamedBufferData(
        _indirect_buffer,
        sizeof(MDICommand) * _commands.size(),
        _commands.data(),
        GL_DYNAMIC_DRAW
    );

    _commands.clear();

    _buffer->set_buffer(_vertices.data(), sizeof(Vertex) * _vertices.size());
    _vertices.clear();

    _atlas->bind();

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _indirect_buffer);
    glMultiDrawElementsIndirect(
        GL_TRIANGLES,    // type
        GL_UNSIGNED_INT, // indices represented as unsigned ints
        (GLvoid*)0,      // start with the first draw command
        total_quads,     // draw n objects
        0                // no stride, the draw commands are tightly packed
    );
}

MeshScreenLog::MeshScreenLog() :
    _faces { nullptr },
    _atlas { nullptr },
    _indirect_buffer { GL_NONE }
{
    _vertices.reserve(1024);
    _commands.reserve(1024);

    _buffer = VertexBuffer::create({
        { "position", VBElement::Type::vec4f },
        { "texcoord", VBElement::Type::vec2f }
    });

    _faces = new Face[QUAD_FACES] {
        { 0, 1, 2 },
        { 2, 3, 0 }
    };

    _buffer->set_indices(_faces, QUAD_FACES);

    ::glCreateBuffers(1, &_indirect_buffer);
    assert(_indirect_buffer != GL_NONE);
}

MeshScreenLog::~MeshScreenLog() {
    delete _buffer;
    delete _faces;
}

} // namespace btx