#ifndef BRASSTACKS_ECS_ENTITY_HPP
#define BRASSTACKS_ECS_ENTITY_HPP

#include "brasstacks/System/pch.hpp"

namespace btx {

constexpr uint8_t MAX_ENTITIES   = 128u;

using EntityID      = std::uint64_t;
using EntityIndex   = std::uint32_t;
using EntityVersion = std::uint32_t;

constexpr EntityID INVALID_ENTITY = ((EntityID)-1 << 32) | ((EntityID)0);

EntityID create_entity_id(EntityIndex index, EntityVersion version) {
    return ((EntityID)index << 32) | ((EntityID)version);
}

EntityIndex get_entity_index(EntityID id) {
    return id >> 32;
}

EntityVersion get_entity_version(EntityID id) {
    return (EntityVersion)id;
}

bool entity_valid(EntityID id) {
    return (id >> 32) != EntityIndex(-1);
}

};

#endif // BRASSTACKS_ECS_ENTITY_HPP