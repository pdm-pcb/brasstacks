#include "brasstacks/rendering/meshes/Vertex.hpp"

namespace btx {

Vertex::Bindings const Vertex::bindings {{
    .binding   = 0u,
    .stride    = sizeof(Vertex),
    .inputRate = vk::VertexInputRate::eVertex
}};

Vertex::Attribs const Vertex::attributes {{
    .location = 0u,
    .binding  = 0u,
    .format   = vk::Format::eR32G32B32Sfloat,
    .offset   = static_cast<uint32_t>(offsetof(Vertex, position)),
},
{
    .location = 1u,
    .binding  = 0u,
    .format   = vk::Format::eR32G32B32Sfloat,
    .offset   = static_cast<uint32_t>(offsetof(Vertex, color)),
}};

} // namespace btx