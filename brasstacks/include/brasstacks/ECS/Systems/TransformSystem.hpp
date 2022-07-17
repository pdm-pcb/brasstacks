#ifndef BRASSTACKS_ECS_SYSTEM_STRANSFORMSYSTEM_HPP
#define BRASSTACKS_ECS_SYSTEM_STRANSFORMSYSTEM_HPP

#include "brasstacks/ECS/Entity.hpp"
#include "brasstacks/ECS/Components/TransformComponent.hpp"
#include "brasstacks/ECS/ComponentArray.hpp"

namespace btx {

class TransformSystem final {
public:
    void add_entity(Entity entity);
    void remove_entity(Entity entity);

    void update();

    TransformComponent & get_transform(Entity entity);

    TransformSystem();
    ~TransformSystem() = default;

    TransformSystem(const TransformSystem &&) = delete;
    TransformSystem(TransformSystem &)        = delete;

    TransformSystem & operator=(const TransformSystem &&) = delete;
    TransformSystem & operator=(TransformSystem &)        = delete;

private:
    ComponentArray<TransformComponent> _transforms;
    std::array<Entity, MAX_ENTITIES>   _entities;
    std::size_t                        _active_entities;
};

} // namespace btx

#endif // BRASSTACKS_ECS_SYSTEM_STRANSFORMSYSTEM_HPP