#ifndef BRASSTACKS_ECS_CUBESYSTEM_HPP
#define BRASSTACKS_ECS_CUBESYSTEM_HPP

#include "brasstacks/System/glm.hpp"

namespace btx {

class ECS;

class CubeSystem {
public:
    static void update(ECS *ecs, const float frame_delta);

private:
    static float rot_factor;
};

} // namespace btx

#endif // BRASSTACKS_ECS_CUBESYSTEM_HPP