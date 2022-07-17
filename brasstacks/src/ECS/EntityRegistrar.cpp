#include "brasstacks/System/pch.hpp"
#include "brasstacks/ECS/EntityRegistrar.hpp"

namespace btx {

std::queue<Entity> EntityRegistrar::_available_ids { };
std::array<Signature, MAX_ENTITIES> EntityRegistrar::_signatures { };
std::uint32_t EntityRegistrar::_active_entities { 0u };

Entity EntityRegistrar::create_entity() {
    assert(_active_entities < MAX_ENTITIES);

    Entity id = _available_ids.front();
    _available_ids.pop();
    ++_active_entities;

    return id;
}

void EntityRegistrar::destroy_entity(Entity entity) {
    assert(entity < MAX_ENTITIES);

    _signatures[entity].reset();
    _available_ids.push(entity);
    --_active_entities;
}

void EntityRegistrar::set_signature(Entity entity, Signature sig) {
    assert(entity < MAX_ENTITIES);
    _signatures[entity] = sig;
}

Signature EntityRegistrar::get_signature(Entity entity) {
    assert(entity < MAX_ENTITIES);
    return _signatures[entity];
}

void EntityRegistrar::init() {
    for(Entity ent = 0; ent < MAX_ENTITIES; ++ent) {
        _available_ids.push(ent);
    }
}

} // namespace btx