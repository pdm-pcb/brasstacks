#ifndef BRASSTACKS_ECS_COMPONENTS_RENDERCOMP_HPP
#define BRASSTACKS_ECS_COMPONENTS_RENDERCOMP_HPP

#include "brasstacks/System/glm.hpp"

namespace btx {

class Shader;
class Mesh;

struct RenderComp {
    Shader   *shader    { nullptr };
    Mesh     *mesh      { nullptr };
    glm::mat4 world_mat { 1.0f };
};

} // namespace brasstacks

#endif // BRASSTACKS_ECS_COMPONENTS_RENDERCOMP_HPP