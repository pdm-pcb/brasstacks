#include "brasstacks/System/pch.hpp"
#include "brasstacks/ECS/Systems/CameraSystem.hpp"

#include "brasstacks/ECS/ECS.hpp"
#include "brasstacks/ECS/ECSView.hpp"
#include "brasstacks/Application/TargetWindow.hpp"

namespace btx {

const glm::vec3 CameraSystem::_forward(0.0f, 0.0f, -1.0f);
const glm::vec3 CameraSystem::_right(1.0f, 0.0f, 0.0f);

void CameraSystem::update(ECS *ecs, const InputState &input,
                          const float frame_delta)
{
    for(const auto ent : ECSView<CameraComponent, TransformComponent,
                                 MovementComponent>(*ecs))
    {
        auto transform = ecs->get<TransformComponent>(ent);
        auto movement  = ecs->get<MovementComponent>(ent);

        movement->speed   = 0.0f;
        movement->direction = { 0.0f, 0.0f, 0.0f };

        if(input.w) {
            movement->speed = 1.5f * frame_delta;
            movement->direction += _forward;
        }
        else if(input.s) {
            movement->speed = 1.5f * frame_delta;
            movement->direction += -_forward;
        }
    
        if(input.d) {
            movement->speed = 1.5f * frame_delta;
            movement->direction += _right;
        }
        else if(input.a) {
            movement->speed = 1.5f * frame_delta;
            movement->direction += -_right;
        }

        transform->position += movement->direction * movement->speed;
    }
}

} // namespace btx