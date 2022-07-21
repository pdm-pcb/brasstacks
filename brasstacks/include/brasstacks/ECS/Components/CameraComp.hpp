#ifndef BRASSTACKS_ECS_COMPONENTS_CAMERACOMP_HPP
#define BRASSTACKS_ECS_COMPONENTS_CAMERACOMP_HPP

#include "brasstacks/System/glm.hpp"

namespace btx {

struct CameraComp {
    glm::mat4 view_matrix = mat4_ident;
    glm::mat4 proj_matrix = mat4_ident;

    glm::vec3 up      = up_vector;
    glm::vec3 right   = right_vector;
    glm::vec3 forward = forward_vector;

    float pitch { 0.0f };
    float yaw   { 0.0f };
};

} // namespace btx

#endif // BRASSTACKS_ECS_COMPONENTS_CAMERACOMP_HPP