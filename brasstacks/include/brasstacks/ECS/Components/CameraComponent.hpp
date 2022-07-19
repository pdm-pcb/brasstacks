#ifndef BRASSTACKS_ECS_COMPONENTS_CAMERACOMPONENT_HPP
#define BRASSTACKS_ECS_COMPONENTS_CAMERACOMPONENT_HPP

#include "brasstacks/System/glm.hpp"

namespace btx {

struct CameraComponent {
    glm::mat4 view_matrix;
    glm::mat4 proj_matrix;
    glm::vec3 lookahead;
};

} // namespace btx

#endif // BRASSTACKS_ECS_COMPONENTS_CAMERACOMPONENT_HPP