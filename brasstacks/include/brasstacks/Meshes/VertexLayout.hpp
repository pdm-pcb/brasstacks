#ifndef BRASSTACKS_MESHES_VERTEXLAYOUT_HPP
#define BRASSTACKS_MESHES_VERTEXLAYOUT_HPP

#include <cstdint>
#include <vector>
#include <string>

namespace btx {

struct VBElement {
	enum class Type : std::uint8_t {
		float32,
		int8,    int16,  int32,
		uint8,   uint16, uint32,
		vec2f,   vec4f,  mat4f,
	};

	std::string _name;
	Type        _type;
	std::uint32_t    _size;
	std::uint32_t    _count;
	std::uint32_t    _offset;
	bool        _per_vertex;

	VBElement(const char *name, const Type type, const bool per_vertex = true);
};

using ElementList = std::initializer_list<VBElement>;

class VertexLayout {
public:
	const std::vector<VBElement> &elements() const {
		return _elements;
	}

    std::uint32_t element_count() const {
        return static_cast<std::uint32_t>(_elements.size());
    }

	std::uint32_t stride() const { return _stride; }

	VertexLayout() = delete;
	VertexLayout(const ElementList &elements);

private:
	std::vector<VBElement> _elements;
	std::uint32_t _stride;

	void calculate_offset_and_stride();
};

} // namespace pdr

#endif // BRASSTACKS_MESHES_VERTEXLAYOUT_HPP