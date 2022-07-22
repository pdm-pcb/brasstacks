#include "brasstacks/System/pch.hpp"
#include "brasstacks/ECS/Systems/CameraSystem.hpp"

#include "brasstacks/ECS/ECS.hpp"
#include "brasstacks/ECS/ECSView.hpp"
#include "brasstacks/Application/TargetWindow.hpp"

namespace btx {

void CameraSystem::update(ECS *ecs, const InputState &input,
                          const float frame_delta)
{
    for(const auto id : ECSView<cCamera, cTransform, cMove>(*ecs)) {
        auto camera = ecs->get<cCamera>(id);
    
        auto movement  = ecs->get<cMove>(id);
        movement->speed   = 0.0f;
        movement->direction = { 0.0f, 0.0f, 0.0f };

        if(input.w) {
            movement->speed = 5.0f * frame_delta;
            movement->direction += camera->forward;
        }
        else if(input.s) {
            movement->speed = 5.0f * frame_delta;
            movement->direction += -camera->forward;
        }
    
        if(input.d) {
            movement->speed = 5.0f * frame_delta;
            movement->direction += camera->right;
        }
        else if(input.a) {
            movement->speed = 5.0f * frame_delta;
            movement->direction += -camera->right;
        }

        auto transform = ecs->get<cTransform>(id);
    
        transform->position += movement->direction * movement->speed;
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