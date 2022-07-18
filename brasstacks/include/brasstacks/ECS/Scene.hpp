#ifndef BRASSTACKS_ECS_REGISTRY_HPP
#define BRASSTACKS_ECS_REGISTRY_HPP

#include "brasstacks/ECS/Entity.hpp"
#include "brasstacks/ECS/Component.hpp"

namespace btx {

struct ComponentPool {
    ComponentPool(std::size_t component_size) {
        _component_size = component_size;
        _data = new char[_component_size * MAX_ENTITIES];
    }

    ~ComponentPool() {
        delete[] _data;
    }

    void* get(std::size_t index) {
        // looking up the component at the desired index
        return _data + index * _component_size;
    }

    char *_data { nullptr };
    std::size_t _component_size { 0 };
};

struct Scene {
    struct Entity {
        EntityID id;
        ComponentMask mask;
    };

    std::vector<Entity>          entities;
    std::vector<EntityIndex>     free_indices;
    std::vector<ComponentPool *> component_pools;

    EntityID new_entity() {
        if(!free_indices.empty()) {
            EntityIndex new_index = free_indices.back();
            free_indices.pop_back();
            EntityVersion version = get_entity_version(entities[new_index].id);
            EntityID new_id = create_entity_id(new_index, version);
            entities[new_index].id = new_id;

            return entities[new_index].id;
        }

        EntityIndex new_index = EntityIndex(entities.size());
        EntityID new_id = create_entity_id(new_index, 0);
        entities.push_back({ new_id, ComponentMask() });
    
        return entities.back().id;
    }

    void destroy_entity(EntityID id) {
        EntityID newID = create_entity_id(
            EntityIndex(-1),
            get_entity_version(id) + 1
        );

        EntityIndex index = get_entity_index(id);

        entities[index].id = newID;
        entities[index].mask.reset(); 
        free_indices.push_back(index);
    }


    template<typename T>
    T* assign(EntityID id) {
        EntityIndex index = get_entity_index(id);

        if(entities[index].id != id) {
            return nullptr;
        }

        ComponentID component_id = get_cid<T>();

        // Not enough component pool
        if(component_pools.size() <= component_id) {
            component_pools.resize(component_id + 1, nullptr);
        }
        // New component, make a new pool
        if(component_pools[component_id] == nullptr) {
            component_pools[component_id] = new ComponentPool(sizeof(T));
        }
        // Looks up the component in the pool, and initializes
        // it with placement new
        T* component = new (component_pools[component_id]->get(index)) T();

        // Set the bit for this component to true and return the
        // created component
        entities[index].mask.set(component_id);
        return component;
    }

    template<typename T>
    T* get(EntityID id) {
        EntityIndex index = get_entity_index(id);

        if(entities[index].id != id) {
            return nullptr;
        }

        ComponentID component_id = get_cid<T>();
        if(!entities[index].mask.test(component_id)) {
            return nullptr;
        }

        T* component = static_cast<T*>(
            component_pools[component_id]->get(index)
        );
        return component;
    }

    template<typename T>
    void remove(EntityID id) {
        EntityIndex index = get_entity_index(id);
        if(entities[index].id != id) {
            return;
        }

        ComponentID component_id = get_cid<T>();
        entities[index].mask.reset(component_id);
    }
};

template<typename... ComponentTypes>
struct SceneView {
    ComponentMask component_mask;
    Scene*        scene { nullptr };
    bool          all   { false   };

    SceneView(Scene& scene) :
        scene { &scene }
    {
        if(sizeof...(ComponentTypes) == 0) {
            all = true;
        }
        else {
        // Unpack the template parameters into an initializer list
        ComponentID component_ids[] = {
            0, get_cid<ComponentTypes>() ...
        };
        for(size_t i = 1; i < (sizeof...(ComponentTypes) + 1); i++)
            component_mask.set(component_ids[i]);
        }
    }

    struct Iterator {
        ComponentMask mask;
        Scene*        scene;
        bool          all;
        EntityIndex   index;
    
        Iterator(Scene *scene, EntityIndex index, ComponentMask mask, bool all):
            scene { scene },
            index { index },
            mask  { mask  },
            all   { all   }
        { }

        EntityID operator*() const {
            return scene->entities[index].id; 
        }

        bool operator==(const Iterator& other) const {
            return index == other.index || index == scene->entities.size();
        }

        bool operator!=(const Iterator& other) const {
            return index != other.index && index != scene->entities.size();
        }

        bool valid_index() {
            return entity_valid(scene->entities[index].id) &&
                   (all || mask == (mask & scene->entities[index].mask));
        }

        Iterator& operator++() {
            do {
                index++;
            } while(index < scene->entities.size() && !valid_index());
            return *this;
        }

    };

    const Iterator begin() const {
        EntityIndex first_index = 0;

        while(first_index < scene->entities.size() &&
             (component_mask != (component_mask &
                                 scene->entities[first_index].mask) ||
              !entity_valid(scene->entities[first_index].id)))
        {
            first_index++;
        }

        return Iterator(scene, first_index, component_mask, all);
    }

    const Iterator end() const {
        return Iterator(
            scene,
            EntityIndex(scene->entities.size()),
            component_mask,
            all
        );
    }
};


} // namespace btx

#endif // BRASSTACKS_ECS_REGISTRY_HPP