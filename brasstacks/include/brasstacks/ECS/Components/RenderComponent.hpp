#ifndef BRASSTACKS_ECS_COMPONENTS_RENDERCOMPONENT
#define BRASSTACKS_ECS_COMPONENTS_RENDERCOMPONENT

#include "brasstacks/System/glm.hpp"

namespace btx {

class Shader;
class Mesh;

struct RenderComponent {
    Shader   *shader    { nullptr };
    Mesh     *mesh      { nullptr };
    glm::mat4 world_mat { 1.0f };
};

} // namespace brasstacks

#endif // BRASSTACKS_ECS_COMPONENTS_RENDERCOMPONENT