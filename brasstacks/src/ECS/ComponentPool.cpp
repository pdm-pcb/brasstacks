#include "brasstacks/System/pch.hpp"
#include "brasstacks/ECS/ComponentPool.hpp"

namespace btx {

void * ComponentPool::get(Entity::Index index) {
    return _data + index * _component_size;
}

ComponentPool::ComponentPool(std::size_t component_size) {
    _component_size = component_size;
    _data = new char[_component_size * MAX_ENTITIES];
}

ComponentPool::~ComponentPool() {
    delete[] _data;
}

} // namespace btx