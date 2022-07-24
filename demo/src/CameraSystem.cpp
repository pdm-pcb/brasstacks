#include "brasstacks/System/pch.hpp"
#include "CameraSystem.hpp"

#include "brasstacks/ECS/ECS.hpp"
#include "brasstacks/ECS/ECSView.hpp"
#include "brasstacks/Application/TargetWindow.hpp"

float CameraSystem::_strafe_speed = 3.0f;
float CameraSystem::_sprint_speed = 9.0f;

void CameraSystem::update(const InputState &input, const float frame_delta) {
    float effective_speed;
    if(input.shift) {
        effective_speed = _sprint_speed;
    }
    else {
        effective_speed = _strafe_speed;
    }

    btx::ECS *ecs = btx::ECS::get_active();

    for(const auto id : btx::ECSView<btx::cCamera,
                                     btx::cTransform,
                                     btx::cMove>(*ecs))
    {
        auto camera = ecs->get<btx::cCamera>(id);
    
        auto movement  = ecs->get<btx::cMove>(id);
        movement->speed   = 0.0f;
        movement->direction = { 0.0f, 0.0f, 0.0f };

        if(input.w) {
            movement->speed = effective_speed * frame_delta;
            movement->direction += camera->forward;
        }
        else if(input.s) {
            movement->speed = effective_speed * frame_delta;
            movement->direction += -camera->forward;
        }
    
        if(input.d) {
            movement->speed = effective_speed * frame_delta;
            movement->direction += camera->right;
        }
        else if(input.a) {
            movement->speed = effective_speed * frame_delta;
            movement->direction += -camera->right;
        }

        if(input.space) {
            movement->speed = effective_speed * frame_delta;
            movement->direction += camera->up;
        }
        else if(input.ctrl) {
            movement->speed = effective_speed * frame_delta;
            movement->direction += -camera->up;
        }

        auto transform = ecs->get<btx::cTransform>(id);
    
        transform->position += movement->direction * movement->speed;
        // transform->position.y = 0.0f;
        transform->rotation = glm::quat({ camera->pitch, -camera->yaw, 0.0f });

        auto T = glm::translate(btx::mat4_ident, transform->position);
        auto R = glm::mat4(transform->rotation);
        auto world = T * R;

        camera->right   = glm::normalize(world * glm::vec4(btx::right_vector,   0.0f));
        camera->forward = glm::normalize(world * glm::vec4(btx::forward_vector, 0.0f));
        camera->up      = glm::normalize(world * glm::vec4(btx::up_vector,      0.0f));

        camera->view_matrix = glm::lookAt(
            transform->position,
            transform->position + camera->forward,
            camera->up
        );

        for(const auto render_id : btx::ECSView<btx::cPhongParams>(*ecs)) {
            auto phong = ecs->get<btx::cPhongParams>(render_id);
            phong->params.camera_pos = { transform->position, 1.0f };
        }
    }
}