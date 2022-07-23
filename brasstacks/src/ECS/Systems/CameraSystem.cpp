#include "brasstacks/System/pch.hpp"
#include "brasstacks/ECS/Systems/CameraSystem.hpp"

#include "brasstacks/ECS/ECS.hpp"
#include "brasstacks/ECS/ECSView.hpp"
#include "brasstacks/Application/TargetWindow.hpp"

namespace btx {

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

    ECS *ecs = btx::ECS::get_active();

    for(const auto id : ECSView<cCamera, cTransform, cMove>(*ecs)) {
        auto camera = ecs->get<cCamera>(id);
    
        auto movement  = ecs->get<cMove>(id);
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

        auto transform = ecs->get<cTransform>(id);
    
        transform->position += movement->direction * movement->speed;
        // transform->position.y = 0.0f;
        transform->rotation = glm::quat({ camera->pitch, -camera->yaw, 0.0f });

        auto T = glm::translate(mat4_ident, transform->position);
        auto R = glm::mat4(transform->rotation);
        auto world = T * R;

        camera->right   = glm::normalize(world * glm::vec4(right_vector,   0.0f));
        camera->forward = glm::normalize(world * glm::vec4(forward_vector, 0.0f));
        camera->up      = glm::normalize(world * glm::vec4(up_vector,      0.0f));

        camera->view_matrix = glm::lookAt(
            transform->position,
            transform->position + camera->forward,
            camera->up
        );

        for(const auto render_id : ECSView<cPhongParams>(*ecs)) {
            auto phong = ecs->get<cPhongParams>(render_id);
            phong->params.camera_pos = { transform->position, 1.0f };
        }
    }
}

} // namespace btx