#ifndef BRASSTACKS_RENDERING_MESHES_VERTEX_HPP
#define BRASSTACKS_RENDERING_MESHES_VERTEX_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/math/math.hpp"
#include "brasstacks/math/colors.hpp"

namespace btx {

struct Vertex final {
    math::Vec3 position { math::Vec3::zero };
    math::Vec3 color    { colors::white };
    math::Vec2 uv       { 0.0f, 0.0f };

    using Bindings = std::vector<vk::VertexInputBindingDescription>;
    static Bindings const bindings;

    using Attributes = std::vector<vk::VertexInputAttributeDescription>;
    static Attributes const attributes;
};

} // namespace btx

#endif // BRASSTACKS_RENDERING_MESHES_VERTEX_HPP