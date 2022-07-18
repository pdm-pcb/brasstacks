#ifndef BRASSTACKS_ECS_COMPONENTARRAY_HPP
#define BRASSTACKS_ECS_COMPONENTARRAY_HPP

#include "brasstacks/ECS/Entity.hpp"
#include "brasstacks/ECS/Component.hpp"

#include <array>
#include <unordered_map>

namespace btx {

template<typename T>
class ComponentArray {
// public:
//     void add_component(Entity entity, T component) {
//         assert(entity < MAX_ENTITIES);
//         assert(_active_components + 1 < MAX_COMPONENTS);

//         _entity_to_index[entity] = _active_components;
//         _index_to_entity[_active_components] = entity;
//         _components[_active_components] = component;
//         ++_active_components;
//     }

//     void remove_component(Entity entity) {
//         assert(_entity_to_index.find(entity) != _entity_to_index.end());

//         std::size_t index_to_remove = _entity_to_index[entity];
//         std::size_t index_to_swap   = _active_components - 1;
//         Entity entity_to_swap       = _index_to_entity[index_to_swap];

//         std::swap(_components[index_to_remove], _components[index_to_swap]);
//         _components[index_to_swap] = { };

//         _entity_to_index[entity_to_swap]  = index_to_remove;
//         _index_to_entity[index_to_remove] = entity_to_swap;

//         _entity_to_index.erase(entity);
//         _index_to_entity.erase(index_to_swap);
    
//         --_active_components;
//     }

//     T & get_component(Entity entity) {
//         assert(_entity_to_index.find(entity) != _entity_to_index.end());
//         std::size_t index = _entity_to_index[entity];
//         return _components[index];
//     }

//     ComponentArray() :
//         _components { },
//         _active_components { 0 }
//     { }

//     ~ComponentArray() = default;

//     ComponentArray(const ComponentArray &&) = delete;
//     ComponentArray(ComponentArray &)        = delete;

//     ComponentArray & operator=(const ComponentArray &&) = delete;
//     ComponentArray & operator=(ComponentArray &)        = delete;

// private:
//     std::array<T, MAX_ENTITIES> _components;
//     std::unordered_map<Entity, std::size_t> _entity_to_index;
//     std::unordered_map<std::size_t, Entity> _index_to_entity;

//     std::size_t _active_components;
};

} // namespace btx

#endif // BRASSTACKS_ECS_COMPONENTARRAY_HPP