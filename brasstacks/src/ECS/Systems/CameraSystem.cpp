#include "brasstacks/System/pch.hpp"
#include "brasstacks/ECS/Systems/CameraSystem.hpp"

#include "brasstacks/ECS/ECS.hpp"
#include "brasstacks/ECS/ECSView.hpp"
#include "brasstacks/Application/TargetWindow.hpp"

namespace btx {

const glm::vec3 CameraSystem::_forward(0.0f, 0.0f, -1.0f);
const glm::vec3 CameraSystem::_right(1.0f, 0.0f, 0.0f);
const glm::vec3 CameraSystem::_up(glm::cross(_forward, _right));
const glm::mat4 CameraSystem::_ident(1.0f);

void CameraSystem::update(ECS *ecs, const InputState &input,
                          const float frame_delta)
{
    for(const auto id : ECSView<CameraComponent,
                                TransformComponent,
                                MovementComponent>(*ecs))
    {
        auto camera = ecs->get<CameraComponent>(id);
    
        auto movement  = ecs->get<MovementComponent>(id);
        movement->speed   = 0.0f;
        movement->direction = { 0.0f, 0.0f, 0.0f };

        if(input.w) {
            movement->speed = 2.5f * frame_delta;
            movement->direction += camera->roll_axis;
        }
        else if(input.s) {
            movement->speed = 2.5f * frame_delta;
            movement->direction += -camera->roll_axis;
        }
    
        if(input.d) {
            movement->speed = 2.5f * frame_delta;
            movement->direction += -camera->pitch_axis;
        }
        else if(input.a) {
            movement->speed = 2.5f * frame_delta;
            movement->direction += camera->pitch_axis;
        }

        auto transform = ecs->get<TransformComponent>(id);
    
        transform->position += movement->direction * movement->speed;
        transform->rotation = glm::quat({ -camera->pitch, camera->yaw, 0.0f });

        auto T = glm::translate(_ident, transform->position);
        auto R = glm::mat4(transform->rotation);
        auto world = T * R;

        camera->pitch_axis = glm::normalize(world * glm::vec4(_right,   0.0f));
        camera->roll_axis  = glm::normalize(world * glm::vec4(_forward, 0.0f));
        camera->yaw_axis   = glm::normalize(world * glm::vec4(_up,      0.0f));

        camera->view_matrix = glm::lookAt(
            transform->position,
            transform->position + glm::vec3(camera->roll_axis),
            glm::vec3(camera->yaw_axis)
        );
    }
}

} // namespace btx