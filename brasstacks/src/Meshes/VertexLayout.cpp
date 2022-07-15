#include "brasstacks/System/pch.hpp"
#include "brasstacks/Meshes/VertexLayout.hpp"

namespace btx {

VBElement::VBElement(const char *name, const Type type, const bool per_vertex) :
    _name       { name },
    _type       { type },
    _offset     { 0u   },
    _per_vertex { per_vertex }
{
    switch(type) {
        case Type::float32: _size = sizeof(float);         _count = 1; break;
        case Type::int8:    _size = sizeof(int8_t);        _count = 1; break;
        case Type::int16:   _size = sizeof(int16_t);       _count = 1; break;
        case Type::int32:   _size = sizeof(int32_t);       _count = 1; break;
        case Type::uint8:   _size = sizeof(uint8_t);       _count = 1; break;
        case Type::uint16:  _size = sizeof(uint16_t);      _count = 1; break;
        case Type::uint32:  _size = sizeof(uint32_t);      _count = 1; break;
        case Type::vec2f:   _size = sizeof(float) * 2;     _count = 1; break;
        case Type::vec4f:   _size = sizeof(float) * 4;     _count = 1; break;
        case Type::mat4f:   _size = sizeof(float) * 4 * 4; _count = 4; break;
    }
}

VertexLayout::VertexLayout(const ElementList &elements) :
    _elements{ elements }, _stride{ 0 }
{
    calculate_offset_and_stride();
}

void VertexLayout::calculate_offset_and_stride() {
    uint32_t offset = 0;
    _stride = 0;

    for(auto &element : _elements) {
        element._offset = offset;
        offset  += element._size;
        _stride += element._size;
    }

    // for GLM_FORCE_DEFAULT_ALIGNED_GENTYPES - later benchmarking?
    // float quotient = _stride / 16.0f;
    // uint32_t next_offset = static_cast<uint32_t>(std::ceil(quotient)) * 16;
    // uint32_t padding = next_offset - _stride;

    // _stride += padding;
}

} // namespace btx