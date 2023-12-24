#include "brasstacks/rendering/meshes/Mesh.hpp"

#include "brasstacks/platform/vulkan/resources/vkBuffer.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"

namespace btx {

Mesh::Mesh(vkDevice const &device) :
    _device { device }
{ }

Mesh::~Mesh() {
    delete _vertex_buffer;
    delete _index_buffer;
}

void Mesh::_set_vertices(std::vector<Vertex> const &vertices) {
    _vertex_data = vertices;

    _vertex_buffer = new vkBuffer(
        _device, sizeof(Vertex) * _vertex_data.size(),
        (vk::BufferUsageFlagBits::eVertexBuffer |
         vk::BufferUsageFlagBits::eTransferDst),
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );

    _vertex_buffer->send_to_device(_vertex_data.data());

    _vertex_offsets.emplace_back(0u);
}

void Mesh::_set_indices(std::vector<Index> const &indices) {
    _index_data = indices;

    _index_buffer = new vkBuffer(
        _device, sizeof(Index) * _index_data.size(),
        (vk::BufferUsageFlagBits::eIndexBuffer |
         vk::BufferUsageFlagBits::eTransferDst),
        vk::MemoryPropertyFlagBits::eDeviceLocal
    );

    _index_buffer->send_to_device(_index_data.data());
};

void Mesh::draw_indexed(vkCmdBuffer const &cmd_buffer) const {
    cmd_buffer.native().bindVertexBuffers(
        0u,                        // First binding
        1u,                        // Binding count
        &_vertex_buffer->native(), // Buffers
        _vertex_offsets.data()     // Offsets
    );
    cmd_buffer.native().bindIndexBuffer(
        _index_buffer->native(), // Buffer
        0u,                      // Offset
        INDEX_TYPE               // Index type
    );
    cmd_buffer.native().drawIndexed(
        static_cast<uint32_t>(_index_data.size()),
        1u, 0u, 0u, 0u
    );
}

} // namespace btx