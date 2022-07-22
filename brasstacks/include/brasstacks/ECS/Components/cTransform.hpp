#ifndef BRASSTACKS_ECS_COMPONENTS_CTRANSFORM_HPP
#define BRASSTACKS_ECS_COMPONENTS_CTRANSFORM_HPP

#include "brasstacks/System/glm.hpp"

namespace btx {

struct cTransform {
    glm::vec3 position { 0.0f, 0.0f, 0.0f };
    glm::quat rotation { 1.0f, 0.0f, 0.0f, 0.0f };
    glm::vec3 scale    { 1.0f, 1.0f, 1.0f };
};

} // namespace btx

#endif // BRASSTACKS_ECS_COMPONENTS_CTRANSFORM_HPP