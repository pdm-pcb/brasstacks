#ifndef BRASSTACKS_RENDERER_MESHES_VERTEX_HPP
#define BRASSTACKS_RENDERER_MESHES_VERTEX_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/math/colors.hpp"

namespace btx {

struct Vertex final {
    math::Vec3 position { math::Vec3::zero };
    math::Vec3 color    { colors::white };

    using Bindings = std::vector<vk::VertexInputBindingDescription>;
    static Bindings const bindings;

    using Attribs  = std::vector<vk::VertexInputAttributeDescription>;
    static Attribs  const attributes;
};

} // namespace btx

#endif // BRASSTACKS_RENDERER_MESHES_VERTEX_HPP