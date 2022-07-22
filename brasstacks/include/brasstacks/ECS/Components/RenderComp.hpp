#ifndef BRASSTACKS_ECS_COMPONENTS_RENDERCOMP_HPP
#define BRASSTACKS_ECS_COMPONENTS_RENDERCOMP_HPP

#include "brasstacks/System/glm.hpp"
#include "brasstacks/Shaders/Shader.hpp"

namespace btx {

class Shader;
class Mesh;

struct RenderComp {
    Shader   *shader    { nullptr };
    Mesh     *mesh      { nullptr };
    glm::mat4 world_mat { 1.0f };
};

struct RenderCompPhongNormalMap : public RenderComp {
    Shader::WorldAndMaterial world_and_material {
        .world    = glm::mat4 { 1.0f },
        .ambient  = glm::vec4 { 0.1f },
        .diffuse  = glm::vec4 { 0.1f },
        .specular = glm::vec4 { 0.1f },
        .shine    = 1.0f,
    };
};

} // namespace brasstacks

#endif // BRASSTACKS_ECS_COMPONENTS_RENDERCOMP_HPP