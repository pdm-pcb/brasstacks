#ifndef BRASSTACKS_ECS_COMPONENT_HPP
#define BRASSTACKS_ECS_COMPONENT_HPP

#include "brasstacks/System/pch.hpp"

namespace btx {

extern std::uint32_t component_counter;

constexpr uint8_t MAX_COMPONENTS = 32u;

using ComponentID   = std::uint32_t;
using ComponentMask = std::bitset<MAX_COMPONENTS>;

template <class T>
ComponentID get_cid() {
    static ComponentID component_id = component_counter++;
    return component_id;
}

} // namespace btx

#endif // BRASSTACKS_ECS_COMPONENT_HPP