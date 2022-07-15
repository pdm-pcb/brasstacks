#ifndef BRASSTACKS_PLATFORM_GL_GLVERTEXBUFFER_HPP
#define BRASSTACKS_PLATFORM_GL_GLVERTEXBUFFER_HPP

#include "brasstacks/Meshes/VertexBuffer.hpp"

namespace btx {

class GLVertexBuffer : public VertexBuffer {
public:
    void bind() override;

    void set_buffer(uint32_t size, uint32_t stride, void *buffer) override;
    void set_indices(const uint32_t *faces, const uint32_t face_count) override;

    void set_layout(const ElementList &elements) override;
    void set_layout(const VertexLayout &layout)  override;

    GLVertexBuffer();

private:
    GLuint _vao;
    GLuint _vbo;
    GLuint _ibo;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_GL_GLVERTEXBUFFER_HPP