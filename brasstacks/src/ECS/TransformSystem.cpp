#include "brasstacks/System/pch.hpp"
#include "brasstacks/ECS/Systems/TransformSystem.hpp"

namespace btx {

void TransformSystem::add_entity(Entity entity) {
    _transforms.add_component(
        entity,
        {
            .position = glm::vec3(0.0f),
            .rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
            .scale    = glm::vec3(1.0f)
        }
    );
    ++_active_entities;
}

void TransformSystem::remove_entity(Entity entity) {
    _transforms.remove_component(entity);
    --_active_entities;
}

void TransformSystem::update() {
    assert(_active_entities > 0);
    for(Entity entity = 0; entity < _active_entities; ++entity) {
        TransformComponent &transform = _transforms.get_component(entity);

        if(transform.scale.x <= 0.01f) continue;

        auto scale_factor = glm::vec3(0.1f) * Clock::frame_delta();
        transform.scale -= scale_factor;
    }
}

TransformComponent & TransformSystem::get_transform(Entity entity) {
    return _transforms.get_component(entity);
}

TransformSystem::TransformSystem() :
    _entities { 0 },
    _active_entities { 0 }
{ }


} // namespace btx