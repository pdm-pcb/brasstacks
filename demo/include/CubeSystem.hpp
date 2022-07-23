#pragma once

#include "brasstacks/System/glm.hpp"

class CubeSystem {
public:
    static void update(const float frame_delta);

private:
    static float _rot_factor;
    static const glm::vec3 _unit_x;
    static const glm::vec3 _unit_y;
    static const glm::vec3 _unit_z;
};