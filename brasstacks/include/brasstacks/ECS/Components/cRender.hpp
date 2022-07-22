#ifndef BRASSTACKS_ECS_COMPONENTS_CRENDER_HPP
#define BRASSTACKS_ECS_COMPONENTS_CRENDER_HPP

#include "brasstacks/System/glm.hpp"
#include "brasstacks/Shaders/Shader.hpp"

namespace btx {

class Shader;
class Mesh;

struct cRender {
    Shader   *shader { nullptr };
    Mesh     *mesh   { nullptr };
};

struct cPhongMaterial {
    glm::vec4 ambient   = { 0.25f, 0.25f, 0.25f, 1.0f };
    glm::vec4 diffuse   = { 0.75f, 0.75f, 0.75f, 1.0f };
    glm::vec4 specular  = { 0.95f, 0.95f, 0.95f, 1.0f };
    float shine = 1.0f;
};

struct cPhongParams {
    Shader::LightParameters params;
};

} // namespace brasstacks

#endif // BRASSTACKS_ECS_COMPONENTS_CRENDER_HPP