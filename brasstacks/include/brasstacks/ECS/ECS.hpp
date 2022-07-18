#ifndef BRASSTACKS_ECS_ECS_HPP
#define BRASSTACKS_ECS_ECS_HPP

#include "brasstacks/ECS/Entity.hpp"
#include "brasstacks/ECS/Component.hpp"
#include "brasstacks/ECS/ComponentPool.hpp"

#include "brasstacks/ECS/Components/TransformComponent.hpp"
#include "brasstacks/ECS/Components/MovementComponent.hpp"
#include "brasstacks/ECS/Components/CameraComponent.hpp"
#include "brasstacks/ECS/Components/CubeComponent.hpp"

namespace btx {

class ECS final {
public:
    Entity::ID new_entity();
    void destroy_entity(Entity::ID id);

    template<typename T>
    T* assign(Entity::ID id) {
        auto index = Entity::get_index(id);

        if(_entities[index].id != id) {
            return nullptr;
        }

        auto component_id = Component::get_id<T>();

        if(_component_pools.size() <= component_id) {
            _component_pools.resize(component_id + 1, nullptr);
        }

        if(_component_pools[component_id] == nullptr) {
            _component_pools[component_id] = new ComponentPool(sizeof(T));
        }

        T* component = new (_component_pools[component_id]->get(index)) T();

        _entities[index].mask.set(component_id);
        return component;
    }

    template<typename T>
    T* get(Entity::ID id) {
    auto index = Entity::get_index(id);

        if(_entities[index].id != id) {
            return nullptr;
        }

        auto component_id = Component::get_id<T>();
        if(!_entities[index].mask.test(component_id)) {
            return nullptr;
        }

        T* component = static_cast<T*>(
            _component_pools[component_id]->get(index)
        );
        return component;
    }

    template<typename T>
    void remove(Entity::ID id) {
        auto index = Entity::get_index(id);
        if(_entities[index].id != id) {
            return;
        }

        auto component_id = Component::get_id<T>();
        _entities[index].mask.reset(component_id);
    }

    Entity & entity(Entity::Index index) { return _entities[index]; }
    std::size_t entity_count() const     { return _entities.size(); }

    ECS() = default;
    ~ECS() = default;

    ECS(const ECS &&) = delete;
    ECS(ECS &)        = delete;
    
    ECS & operator=(const ECS &&) = delete;
    ECS & operator=(ECS &)        = delete;    

private:
    std::vector<Entity>          _entities;
    std::vector<Entity::Index>   _free_indices;
    std::vector<ComponentPool *> _component_pools;
};

} // namespace btx

#endif // BRASSTACKS_ECS_ECS_HPP