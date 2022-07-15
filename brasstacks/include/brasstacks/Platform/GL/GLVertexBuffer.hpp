#ifndef BRASSTACKS_PLATFORM_GL_GLVERTEXBUFFER_HPP
#define BRASSTACKS_PLATFORM_GL_GLVERTEXBUFFER_HPP

#include "brasstacks/Meshes/VertexBuffer.hpp"

namespace btx {

class GLVertexBuffer : public VertexBuffer {
public:
    void bind() override;

    void set_buffer(void *buffer, uint32_t size) override;
    void set_indices(const Mesh::Face *faces,
                     const uint32_t face_count)  override;

    GLVertexBuffer(const ElementList &elements);

private:
    GLuint _vao;
    GLuint _vbo;
    GLuint _ibo;

    VertexLayout *_layout;

    void _set_layout();
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_GL_GLVERTEXBUFFER_HPP