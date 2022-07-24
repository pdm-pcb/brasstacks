#include "brasstacks/System/pch.hpp"
#include "brasstacks/Platform/GL/GLVertexBuffer.hpp"

namespace btx {

void GLVertexBuffer::bind() {
    glBindVertexArray(_vao);
}

void GLVertexBuffer::set_buffer(void *buffer, std::size_t size) {
    glCreateBuffers(1, &_vbo);
    
    if(_vbo == GL_NONE) {
        BTX_ENGINE_ERROR("glCreateBuffers() failed for VBO");
        assert(false);
    }

    glNamedBufferStorage(
        _vbo,
        static_cast<GLsizeiptr>(size),
        buffer,
        GL_DYNAMIC_STORAGE_BIT
    );
    glVertexArrayVertexBuffer(
        _vao, 0,          // vaobj, binding index
        _vbo, 0,          // buffer, offset
        _layout->stride() // stride
    );
}

void GLVertexBuffer::set_indices(const Mesh::Face *faces,
                                 const std::size_t face_count) {
    glCreateBuffers(1, &_ibo);
    
    if(_ibo == GL_NONE) {
        BTX_ENGINE_ERROR("glCreateBuffers() failed for IBO");
        assert(false);
    }

    glNamedBufferStorage(
        _ibo,
        sizeof(Mesh::Face) * face_count,
        faces,
        GL_DYNAMIC_STORAGE_BIT
    );
    glVertexArrayElementBuffer(_vao, _ibo);
}

void GLVertexBuffer::_set_layout() {
	std::uint32_t elem_count   = 0;
    std::uint32_t layout_count = 0;
	std::uint32_t data_type    = GL_NONE;

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
					glVertexArrayBindingDivisor(_vao, layout_count, 1);
				}
                glVertexArrayAttribBinding(_vao, layout_count, 0);
				++layout_count;
				break;

			case type::mat4f:
				for(std::uint32_t column = 0; column < element._count; column++) {
                    glEnableVertexArrayAttrib(_vao, layout_count);
                    glVertexArrayAttribFormat(
                        _vao,
                        layout_count,
                        elem_count,
                        data_type, GL_FALSE,
                        element._offset
                    );
					glVertexArrayBindingDivisor(_vao, layout_count, 1);
                    glVertexArrayAttribBinding(_vao, layout_count, 0);
					++layout_count;
				}
				break;

            default:
                BTX_ENGINE_ERROR("Unknown OpenGL element type.");
                break;
		}
	}
}

GLVertexBuffer::GLVertexBuffer(const VertexLayout::ElementList &elements) :
    VertexBuffer(),
    _vao    { GL_NONE },
    _vbo    { GL_NONE },
    _ibo    { GL_NONE },
    _layout { new VertexLayout { elements } }
{
    glCreateVertexArrays(1, &_vao);

    if(_vao == GL_NONE) {
        BTX_ENGINE_ERROR("glCreateVertexArrays() failed");
        assert(false);
    }

    _set_layout();
}

GLVertexBuffer::GLVertexBuffer(VertexLayout *layout) :
    VertexBuffer(),
    _vao    { GL_NONE },
    _vbo    { GL_NONE },
    _ibo    { GL_NONE },
    _layout { layout  }
{
    glCreateVertexArrays(1, &_vao);

    if(_vao == GL_NONE) {
        BTX_ENGINE_ERROR("glCreateVertexArrays() failed");
        assert(false);
    }

    _set_layout();
}

GLVertexBuffer::~GLVertexBuffer() {
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_ibo);
}

} // namespace btx