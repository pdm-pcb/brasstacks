#include "brasstacks/System/pch.hpp"
#include "brasstacks/ECS/Entity.hpp"

namespace btx {
Entity::ID Entity::create_id(Entity::Index index, Entity::Version version) {
    return (static_cast<Entity::ID>(index) << 32) |
            (static_cast<Entity::ID>(version));
}

bool Entity::valid(Entity::ID id) {
    return (id >> 32) != static_cast<Entity::Index>(-1);
}

Entity::Index Entity::get_index(Entity::ID id) {
    return id >> 32;
}

Entity::Version Entity::get_version(Entity::ID id) {
    return static_cast<Entity::Version>(id);
}
} // namespace btx