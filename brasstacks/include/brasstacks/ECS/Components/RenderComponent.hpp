#ifndef BRASSTACKS_ECS_COMPONENTS_RENDERCOMPONENT
#define BRASSTACKS_ECS_COMPONENTS_RENDERCOMPONENT

#include "brasstacks/System/glm.hpp"
#include "brasstacks/Meshes/Mesh.hpp"
#include "brasstacks/Meshes/MeshFlatColor.hpp"

namespace btx {

class VertexBuffer;
class ShaderFlatColor;

struct RenderComponent {
    ShaderFlatColor *shader { nullptr };
    glm::mat4 world_mat     { 1.0f };

    MeshFlatColor::Vertex *vertices { nullptr };
    std::uint32_t vertex_count      { 0 };

    Mesh::Face   *faces      { nullptr };
    std::uint32_t face_count { 0 };

    VertexBuffer *vb { nullptr };
};

} // namespace brasstacks

#endif // BRASSTACKS_ECS_COMPONENTS_RENDERCOMPONENT