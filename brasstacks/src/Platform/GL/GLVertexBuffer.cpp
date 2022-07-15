#include "brasstacks/System/pch.hpp"
#include "brasstacks/Platform/GL/GLVertexBuffer.hpp"

namespace btx {

void GLVertexBuffer::bind() {
    glBindVertexArray(_vao);
}

void GLVertexBuffer::set_buffer(void *buffer, uint32_t size) {
    if(_vao == GL_NONE) {
        BTX_ENGINE_ERROR("Must create VAO before VBO");
        return;
    }
    if(_layout == nullptr) {    // TODO: Perhaps I should have the layout as a constructor argument, then...
        BTX_ENGINE_ERROR("Must set vertex layout before assigning data")
        return;
    }

    glCreateBuffers(1, &_vbo);
    
    if(_vbo == GL_NONE) {
        BTX_ENGINE_ERROR("glCreateBuffers() failed for VBO");
        return;
    }

    glNamedBufferStorage(
        _vbo,
        size,
        buffer,
        GL_DYNAMIC_STORAGE_BIT
    );
    glVertexArrayVertexBuffer(
        _vao, 0,          // vaobj, binding index
        _vbo, 0,          // buffer, offset
        _layout->stride() // stride
    );
}

void GLVertexBuffer::set_indices(const uint32_t *indices,
                                 const uint32_t index_count) {
    if(_vao == GL_NONE) {
        BTX_ENGINE_ERROR("Must create VAO before IBO");
        return;
    }

    glCreateBuffers(1, &_ibo);
    
    if(_ibo == GL_NONE) {
        BTX_ENGINE_ERROR("glCreateBuffers() failed for IBO");
        return;
    }

    glNamedBufferStorage(
        _ibo,
        sizeof(uint32_t) * index_count,
        indices,
        GL_DYNAMIC_STORAGE_BIT
    );
    glVertexArrayElementBuffer(_vao, _ibo);
}

void GLVertexBuffer::set_layout(const ElementList &elements) {
    _layout = new VertexLayout { elements };
    _set_layout();
}

void GLVertexBuffer::_set_layout() {
	uint32_t elem_count   = 0;
    uint32_t layout_count = 0;
	uint32_t data_type    = GL_NONE;

    using type = VBElement::Type;

	for(const auto &element : _layout->elements()) {
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
    _vao { GL_NONE }, _vbo { GL_NONE }, _ibo { GL_NONE },
    _layout { nullptr }
{
    glCreateVertexArrays(1, &_vao);

    if(_vao == GL_NONE) {
        BTX_ENGINE_ERROR("glCreateVertexArrays() failed");
    }
}

} // namespace btx