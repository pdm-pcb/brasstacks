#include "brasstacks/System/pch.hpp"
#include "CubeSystem.hpp"

float CubeSystem::_rot_factor = 0.0f;

const glm::vec3 CubeSystem::_unit_x(1.0f, 0.0f, 0.0f);
const glm::vec3 CubeSystem::_unit_y(0.0f, 1.0f, 0.0f);
const glm::vec3 CubeSystem::_unit_z(0.0f, 0.0f, 1.0f);

void CubeSystem::update(const float frame_delta) {
    _rot_factor += 0.2f * frame_delta;

    btx::ECS *ecs = btx::ECS::get_active();
    for(const auto id : btx::ECSView<btx::cCube,
                                     btx::cTransform,
                                     btx::cRender>(*ecs))
    {
        auto x = glm::angleAxis(0.5f * _rot_factor, _unit_x);
        auto y = glm::angleAxis(1.0f * _rot_factor, _unit_y);
        auto z = glm::angleAxis(2.0f * _rot_factor, _unit_z);

        auto transform = ecs->get<btx::cTransform>(id);
        transform->rotation = x * y * z;

        auto T = glm::translate(glm::mat4(1.0f), transform->position);
        auto R = glm::mat4(transform->rotation);
        auto S = glm::scale(transform->scale);

        auto *world = ecs->get<btx::cWorldMat>(id);
        world->world_mat = T * R * S;
    }
}