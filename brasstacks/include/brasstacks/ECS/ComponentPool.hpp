#ifndef BRASSTACKS_ECS_COMPONENTPOOL_HPP
#define BRASSTACKS_ECS_COMPONENTPOOL_HPP

#include "brasstacks/ECS/Entity.hpp"

namespace btx {

class ComponentPool {
public:
    void * get(Entity::Index index);

    ComponentPool(std::size_t component_size);
    ~ComponentPool();

    ComponentPool() = delete;

    ComponentPool(const ComponentPool &&) = delete;
    ComponentPool(ComponentPool &)        = delete;

    ComponentPool & operator=(const ComponentPool &&) = delete;
    ComponentPool & operator=(ComponentPool &)        = delete;

private:
    char *_data { nullptr };
    std::size_t _component_size { 0 };
};

} // namespace btx

#endif // BRASSTACKS_ECS_COMPONENTPOOL_HPP
