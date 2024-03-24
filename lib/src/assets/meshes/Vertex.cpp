#include "brasstacks/assets/meshes/Vertex.hpp"

namespace btx {

Vertex::Bindings const Vertex::bindings {{
    .binding   = 0u,
    .stride    = sizeof(Vertex),
    .inputRate = vk::VertexInputRate::eVertex
}};

Vertex::Attributes const Vertex::attributes {
    vk::VertexInputAttributeDescription {
        .location = 0u,
        .binding  = 0u,
        .format   = vk::Format::eR32G32B32Sfloat,
        .offset   = static_cast<uint32_t>(offsetof(Vertex, position)),
    },
    vk::VertexInputAttributeDescription {
        .location = 1u,
        .binding  = 0u,
        .format   = vk::Format::eR32G32B32Sfloat,
        .offset   = static_cast<uint32_t>(offsetof(Vertex, color)),
    },
    vk::VertexInputAttributeDescription {
        .location = 2u,
        .binding  = 0u,
        .format   = vk::Format::eR32G32Sfloat,
        .offset   = static_cast<uint32_t>(offsetof(Vertex, uv)),
    }
};

} // namespace btx