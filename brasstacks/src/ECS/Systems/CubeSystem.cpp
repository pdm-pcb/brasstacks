#include "brasstacks/System/pch.hpp"
#include "brasstacks/ECS/Systems/CubeSystem.hpp"

#include "brasstacks/ECS/ECS.hpp"
#include "brasstacks/ECS/ECSView.hpp"
#include "brasstacks/Engine/RenderQueue.hpp"
#include "brasstacks/Shaders/ShaderFlatColor.hpp"

namespace btx {

float CubeSystem::_rot_factor = 0.0f;

glm::vec3 unit_x(1.0f, 0.0f, 0.0f);
glm::vec3 unit_y(0.0f, 1.0f, 0.0f);
glm::vec3 unit_z(0.0f, 0.0f, 1.0f);

void CubeSystem::update(ECS *ecs, const float frame_delta) {
    _rot_factor += 0.2f * frame_delta;

    for(const auto id : ECSView<CubeComponent,
                                TransformComponent,
                                RenderComponent>(*ecs))
    {
        auto transform = ecs->get<TransformComponent>(id);

        auto x = glm::angleAxis(0.5f * _rot_factor, unit_x);
        auto y = glm::angleAxis(1.0f * _rot_factor, unit_y);
        auto z = glm::angleAxis(2.0f * _rot_factor, unit_z);

        transform->rotation = x * y * z;

        if(transform->scale.x > 0.05f) {
            transform->scale -= glm::vec3(0.001f);
        }

        auto T = glm::translate(glm::mat4(1.0f), transform->position);
        auto R = glm::mat4(transform->rotation);
        auto S = glm::scale(transform->scale);

        auto render = ecs->get<RenderComponent>(id);
        render->world_mat = T * R * S;

        RenderQueue::submit(dynamic_cast<Shader *>(render->shader), id);
    }
}

} // namespace btx