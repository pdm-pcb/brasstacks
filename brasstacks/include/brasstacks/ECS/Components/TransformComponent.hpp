#ifndef BRASSTACKS_ECS_COMPONENTS_TRANSFORMCOMPONENT_HPP
#define BRASSTACKS_ECS_COMPONENTS_TRANSFORMCOMPONENT_HPP

#include "brasstacks/System/glm.hpp"

namespace btx {

struct TransformComponent {
    glm::vec3 position { };
    glm::quat rotation { };
    glm::vec3 scale    { };
};

} // namespace btx

#endif // BRASSTACKS_ECS_COMPONENTS_TRANSFORMCOMPONENT_HPP