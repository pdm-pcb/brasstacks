#include "brasstacks/System/pch.hpp"
#include "brasstacks/Platform/GL/GLVertexBuffer.hpp"

namespace btx {

void GLVertexBuffer::bind() {
    glBindVertexArray(_vao);
}

void GLVertexBuffer::set_buffer(uint32_t size, uint32_t stride, void *buffer) {
    glCreateBuffers(1, &_vbo);
    glNamedBufferStorage(
        _vbo,
        size,
        buffer,
        GL_DYNAMIC_STORAGE_BIT
    );
    glVertexArrayVertexBuffer(
        _vao,       // vaobj
        0,          // bindingindex
        _vbo,       // buffer
        0,          // offset
        stride      // stride
    );
}

void GLVertexBuffer::set_indices(const uint32_t *indices,
                                 const uint32_t index_count) {
    glCreateBuffers(1, &_ibo);
    glNamedBufferStorage(
        _ibo,
        sizeof(uint32_t) * index_count,
        indices,
        GL_DYNAMIC_STORAGE_BIT
    );
    glVertexArrayElementBuffer(_vao, _ibo);
}

void GLVertexBuffer::set_layout(const ElementList &elements) {
    VertexLayout layout { elements };
    set_layout(layout);
}

void GLVertexBuffer::set_layout(const VertexLayout &layout) {
	uint32_t elem_count   = 0;
    uint32_t layout_count = 0;
	uint32_t data_type    = GL_NONE;

    using type = VBElement::Type;

	for(const auto &element : layout.elements()) {
        switch(element._type) {
            case type::float32: elem_count = 1; data_type = GL_FLOAT;          break;
            case type::int8:    elem_count = 1; data_type = GL_BYTE;           break;
            case type::int16:   elem_count = 1; data_type = GL_SHORT;          break;
            case type::int32:   elem_count = 1; data_type = GL_INT;            break;
            case type::uint8:   elem_count = 1; data_type = GL_UNSIGNED_BYTE;  break;
            case type::uint16:  elem_count = 1; data_type = GL_UNSIGNED_SHORT; break;
            case type::uint32:  elem_count = 1; data_type = GL_UNSIGNED_INT;   break;
            case type::vec2f:   elem_count = 2; data_type = GL_FLOAT;          break;
            case type::vec4f:   elem_count = 4; data_type = GL_FLOAT;          break;
            case type::mat4f:   elem_count = 4; data_type = GL_FLOAT;          break;
            default:
                BTX_ENGINE_ERROR("Unknown OpenGL element type.");
                break;
        }

		switch(element._type) {
            case type::float32:
            case type::int8:
            case type::int16:
            case type::int32:
            case type::uint8:
            case type::uint16:
            case type::uint32:
            case type::vec2f:
            case type::vec4f:
                glEnableVertexArrayAttrib(_vao, layout_count);
                glVertexArrayAttribFormat(
                    _vao,
                    layout_count,
                    elem_count,
                    data_type, GL_FALSE,
                    element._offset
                );
				if(element._per_vertex == false) {
					glVertexAttribDivisor(layout_count, 1);
				}
				layout_count++;
				break;

			case type::mat4f:
				for(uint32_t column = 0; column < element._count; column++) {
                    glEnableVertexArrayAttrib(_vao, layout_count);
                    glVertexArrayAttribFormat(
                        _vao,
                        layout_count,
                        elem_count,
                        data_type, GL_FALSE,
                        element._offset
                    );
					glVertexAttribDivisor(layout_count, 1);
					layout_count++;
				}
				break;
		}
	}
}

GLVertexBuffer::GLVertexBuffer() :
    _vao { 0 }, _vbo { 0 }, _ibo { 0 }
{
    glCreateVertexArrays(1, &_vao);
}

} // namespace btx