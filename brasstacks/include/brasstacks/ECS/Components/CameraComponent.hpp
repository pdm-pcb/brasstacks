#ifndef BRASSTACKS_ECS_COMPONENTS_CAMERACOMPONENT_HPP
#define BRASSTACKS_ECS_COMPONENTS_CAMERACOMPONENT_HPP

#include "brasstacks/System/glm.hpp"

namespace btx {

struct CameraComponent {
    glm::mat4 view_matrix { 1.0f };
    glm::mat4 proj_matrix { 1.0f };
    glm::vec3 lookahead   { 0.0f };

    float pitch { 0.0f };
    float yaw   { 0.0f };

    glm::vec3 pitch_axis { 1.0f, 0.0f, 0.0f };
    glm::vec3 yaw_axis   { 0.0f, 1.0f, 0.0f };
    glm::vec3 roll_axis  { 0.0f, 0.0f, 1.0f };
};

} // namespace btx

#endif // BRASSTACKS_ECS_COMPONENTS_CAMERACOMPONENT_HPP