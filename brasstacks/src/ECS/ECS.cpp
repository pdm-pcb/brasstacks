#include "brasstacks/System/pch.hpp"
#include "brasstacks/ECS/ECS.hpp"

namespace btx {

ECS *ECS::_active;

Entity::ID ECS::new_entity() {
    if(!_free_indices.empty()) {
        Entity::Index new_index = _free_indices.back();
        _free_indices.pop_back();
        auto version = Entity::get_version(_entities[new_index].id);
        auto new_id  = Entity::create_id(new_index, version);
        _entities[new_index].id = new_id;

        return _entities[new_index].id;
    }

    auto new_index = Entity::Index(_entities.size());
    auto new_id    = Entity::create_id(new_index, 0);
    _entities.push_back({ new_id, Component::Mask() });

    return _entities.back().id;
}

void ECS::destroy_entity(Entity::ID id) {
    Entity::ID new_id = Entity::create_id(
        Entity::Index(-1),
        Entity::get_version(id) + 1
    );

    Entity::Index index = Entity::get_index(id);

    _entities[index].id = new_id;
    _entities[index].mask.reset(); 
    _free_indices.push_back(index);
}

Entity & ECS::entity(Entity::Index index) {
    return _entities[index];
}

Entity & ECS::entity(Entity::ID id) {
    Entity::Index index = Entity::get_index(id);
    return _entities[index];
}

std::size_t ECS::entity_count() {
    return _entities.size();
}

} // namespace btx