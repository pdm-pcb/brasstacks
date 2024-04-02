#include "brasstacks/brasstacks.hpp"
#include "brasstacks/assets/meshes/Mesh.hpp"

#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"

namespace btx {

// =============================================================================
Mesh::Mesh() :
    _vertex_buffer  { std::make_unique<vmaBuffer>() },
    _vertex_data    { },
    _vertexs { },
    _index_buffer   { std::make_unique<vmaBuffer>() },
    _index_data     { }
{ }

// =============================================================================
void Mesh::_set_vertices(std::span<Vertex const> const vertices) {
    _vertex_data = { vertices.begin(), vertices.end() };

    _vertex_buffer->create(sizeof(Vertex) * _vertex_data.size(),
                           (vk::BufferUsageFlagBits::eVertexBuffer |
                            vk::BufferUsageFlagBits::eTransferDst),
                            { },
                            vk::MemoryPropertyFlagBits::eDeviceLocal);

    _vertex_buffer->send_to_device(_vertex_data.data());

    _vertexs.emplace_back(0u);
}

// =============================================================================
void Mesh::_set_indices(std::span<Index const> const indices) {
    _index_data = { indices.begin(), indices.end() };

    _index_buffer->create(sizeof(Index) * _index_data.size(),
                          (vk::BufferUsageFlagBits::eIndexBuffer |
                           vk::BufferUsageFlagBits::eTransferDst),
                          { },
                          vk::MemoryPropertyFlagBits::eDeviceLocal);

    _index_buffer->send_to_device(_index_data.data());
};

// =============================================================================
void Mesh::draw_indexed(vkCmdBuffer const &cmd_buffer) const {
    cmd_buffer.native().bindVertexBuffers(
        0u,                        // First binding
        1u,                        // Binding count
        &_vertex_buffer->native(), // Buffers
        _vertexs.data()     // Offsets
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