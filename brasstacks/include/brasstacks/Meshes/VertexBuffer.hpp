#ifndef BRASSTACKS_MESHES_VERTEXBUFFER_HPP
#define BRASSTACKS_MESHES_VERTEXBUFFER_HPP

#include "brasstacks/Meshes/VertexLayout.hpp"
#include "brasstacks/Meshes/Mesh.hpp"
#include <cstdint>

namespace btx {

class VertexBuffer {
public:
    virtual void bind() = 0;

    virtual void set_buffer(void *buffer, std::size_t size) = 0;
    virtual void set_indices(const Mesh::Face *faces,
                             const std::size_t face_count) = 0;

    static VertexBuffer * create(const VertexLayout::ElementList &elements);

    virtual ~VertexBuffer() = default;

    VertexBuffer(const VertexBuffer &&) = delete;
    VertexBuffer(VertexBuffer &)        = delete;

    VertexBuffer & operator=(const VertexBuffer &&) = delete;
    VertexBuffer & operator=(VertexBuffer &)        = delete;

protected:
    VertexBuffer() = default;
};

} // namespace btx

#endif // BRASSTACKS_MESHES_VERTEXBUFFER_HPP