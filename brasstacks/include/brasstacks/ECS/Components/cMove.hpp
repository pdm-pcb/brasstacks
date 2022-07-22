#ifndef BRASSTACKS_ECS_COMPONENTS_CMOVE_HPP
#define BRASSTACKS_ECS_COMPONENTS_CMOVE_HPP

#include "brasstacks/System/glm.hpp"

namespace btx {

struct cMove {
    glm::vec3 direction { 0.0f, 0.0f, -1.0f };
    float speed         { 0.0f };
};

} // namespace btx

#endif // BRASSTACKS_ECS_COMPONENTS_CMOVE_HPP