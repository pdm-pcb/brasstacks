#ifndef BRASSTACKS_ECS_CUBESYSTEM_HPP
#define BRASSTACKS_ECS_CUBESYSTEM_HPP

#include "brasstacks/System/glm.hpp"

namespace btx {

class ECS;

class CubeSystem {
public:
    static void update(ECS *ecs, const float frame_delta);

private:
    static float _rot_factor;
    static const glm::vec3 _unit_x;
    static const glm::vec3 _unit_y;
    static const glm::vec3 _unit_z;
};

} // namespace btx

#endif // BRASSTACKS_ECS_CUBESYSTEM_HPP