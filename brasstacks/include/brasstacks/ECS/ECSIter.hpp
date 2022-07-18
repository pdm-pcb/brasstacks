#ifndef BRASSTACKS_ECS_ECSITER_HPP
#define BRASSTACKS_ECS_ECSITER_HPP

#include "brasstacks/ECS/Entity.hpp"
#include "brasstacks/ECS/Component.hpp"

namespace btx {

class ECS;

class ECSIter {
public:
    Entity::ID operator*() const;
    ECSIter & operator++();
    bool operator==(const ECSIter& other) const;
    bool operator!=(const ECSIter& other) const;

    ECSIter(ECS *ecs, const Entity::Index index, const Component::Mask mask,
            const bool all);

    ~ECSIter() = default;
    
    ECSIter() = delete;

    ECSIter(const ECSIter &&) = delete;
    ECSIter(ECSIter &)        = delete;

    ECSIter & operator=(const ECSIter &&) = delete;
    ECSIter & operator=(ECSIter &)        = delete;

private:
    Component::Mask _mask;
    ECS*            _ecs;
    bool            _all;
    Entity::Index   _index;

    bool _valid_index();
};

};

#endif // BRASSTACKS_ECS_ECSITER_HPP