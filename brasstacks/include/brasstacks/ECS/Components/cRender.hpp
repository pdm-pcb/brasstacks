#ifndef BRASSTACKS_ECS_COMPONENTS_CRENDER_HPP
#define BRASSTACKS_ECS_COMPONENTS_CRENDER_HPP

#include "brasstacks/System/glm.hpp"
#include "brasstacks/Shaders/Shader.hpp"

namespace btx {

class Shader;
class Mesh;

struct cRender {
    Shader   *shader    { nullptr };
    Mesh     *mesh      { nullptr };
};

struct cWorldMat {
    glm::mat4 world_mat { 1.0f };
};

struct cPhongNormalMap {
    Shader::WorldAndMaterial world_and_material {
        .world_mat = glm::mat4 { 1.0f },
        .ambient   = glm::vec4 { 0.1f },
        .diffuse   = glm::vec4 { 0.1f },
        .specular  = glm::vec4 { 0.1f },
        .shine     = 1.0f,
    };

    Shader::LightParameters light_params { };
};

} // namespace brasstacks

#endif // BRASSTACKS_ECS_COMPONENTS_CRENDER_HPP