#ifndef BRASSTACKS_PLATFORM_GL_GLVERTEXBUFFER_HPP
#define BRASSTACKS_PLATFORM_GL_GLVERTEXBUFFER_HPP

#include "brasstacks/Meshes/VertexBuffer.hpp"

namespace btx {

class GLVertexBuffer : public VertexBuffer {
public:
    void bind() override;
    void update_buffer(const void *data, const std::size_t size,
                       const std::size_t offset = 0) override;

    void set_buffer(const void *buffer, std::size_t size) override;
    void set_indices(const Mesh::Face *faces,
                     const std::size_t face_count)  override;

    GLVertexBuffer(const VertexLayout::ElementList &elements);
    GLVertexBuffer(VertexLayout *layout);

    ~GLVertexBuffer();

    GLVertexBuffer() = delete;
    GLVertexBuffer(const GLVertexBuffer &&) = delete;
    GLVertexBuffer(GLVertexBuffer &)        = delete;
    GLVertexBuffer & operator=(const GLVertexBuffer &&) = delete;
    GLVertexBuffer & operator=(GLVertexBuffer &)        = delete;

private:
    GLuint _vao;    // TODO: use VAOs as a per-vertex-layout thing, not a 
    GLuint _vbo;    //       per-mesh thing
    GLuint _ibo;

    VertexLayout *_layout;

    void _set_layout();
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_GL_GLVERTEXBUFFER_HPP