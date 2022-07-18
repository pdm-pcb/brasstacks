#include "brasstacks/System/pch.hpp"
#include "brasstacks/ECS/ECSIter.hpp"

#include "brasstacks/ECS/ECS.hpp"

namespace btx {

Entity::ID ECSIter::operator*() const {
    return _ecs->entity(_index).id; 
}

ECSIter & ECSIter::operator++() {
    do {
        _index++;
    } while(_index < _ecs->entity_count() && !_valid_index());
    return *this;
}

bool ECSIter::operator==(const ECSIter& other) const {
    return _index == other._index || _index == _ecs->entity_count();
}

bool ECSIter::operator!=(const ECSIter& other) const {
    return _index != other._index && _index != _ecs->entity_count();
}

bool ECSIter::_valid_index() {
    return Entity::valid(_ecs->entity(_index).id) &&
            (_all || _mask == (_mask & _ecs->entity(_index).mask));
}

ECSIter::ECSIter(ECS *ecs,
                 const Entity::Index index,
                 const Component::Mask mask,
                 const bool all):
    _ecs   { ecs   },
    _index { index },
    _mask  { mask  },
    _all   { all   }
{ }

} // namespace btx