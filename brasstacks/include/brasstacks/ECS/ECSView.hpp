#ifndef BRASSTACKS_ECS_ECSVIEW_HPP
#define BRASSTACKS_ECS_ECSVIEW_HPP

#include "brasstacks/ECS/Entity.hpp"
#include "brasstacks/ECS/Component.hpp"
#include "brasstacks/ECS/ECSIter.hpp"

namespace btx {

class ECS;
class ECSIter;

template<typename... ComponentTypes>
class ECSView {
public:
    const ECSIter begin() const {
        Entity::Index first_index = 0;

        while(first_index < _ecs->entity_count() &&
            (_mask != (_mask & _ecs->entity(first_index).mask) ||
            !Entity::valid(_ecs->entity(first_index).id)))
        {
            first_index++;
        }

        return ECSIter(_ecs, first_index, _mask, _all);
    }

    const ECSIter end() const {
        return ECSIter(
            _ecs,
            Entity::Index(_ecs->entity_count()),
            _mask,
            _all
        );
    }

    ECSView(ECS &ecs) :
        _ecs { &ecs  },
        _all { false }
    {
        if constexpr(sizeof...(ComponentTypes) == 0) {
            _all = true;
        }
        else {
            Component::ID component_ids[] = {
                0, Component::get_id<ComponentTypes>() ...
            };
            for(size_t i = 1; i < (sizeof...(ComponentTypes) + 1); i++) {
                _mask.set(component_ids[i]);
            }
        }
    }

private:
    Component::Mask _mask;
    ECS            *_ecs;
    bool            _all;
};

};

#endif // BRASSTACKS_ECS_ECSVIEW_HPP