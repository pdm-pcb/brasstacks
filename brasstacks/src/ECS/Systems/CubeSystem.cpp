#include "brasstacks/System/pch.hpp"
#include "brasstacks/ECS/Systems/CubeSystem.hpp"

#include "brasstacks/ECS/ECS.hpp"
#include "brasstacks/ECS/ECSView.hpp"

namespace btx {

float CubeSystem::rot_factor = 0.0f;

void CubeSystem::update(ECS *ecs, const float frame_delta) {
    for(const auto ent : ECSView<CubeComponent, TransformComponent>(*ecs)) {
        auto transform = ecs->get<TransformComponent>(ent);
        
        rot_factor += 0.2f * frame_delta;

        auto x = glm::angleAxis(0.5f * rot_factor, glm::vec3(1.0f, 0.0f, 0.0f));
        auto y = glm::angleAxis(rot_factor, glm::vec3(0.0f, 1.0f, 0.0f));
        auto z = glm::angleAxis(2.0f * rot_factor, glm::vec3(0.0f, 0.0f, 1.0f));

        transform->rotation = x * y * z;

        if(transform->scale.x <= 0.05f) continue;
        transform->scale -= glm::vec3(0.001f);
    }
}

} // namespace btx