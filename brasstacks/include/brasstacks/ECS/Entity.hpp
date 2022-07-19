#ifndef BRASSTACKS_ECS_ENTITY_HPP
#define BRASSTACKS_ECS_ENTITY_HPP

#include "brasstacks/ECS/Component.hpp"

#include <cstdint>

namespace btx {

struct Entity {
    using ID      = std::uint64_t;
    using Index   = std::uint32_t;
    using Version = std::uint32_t;

    static ID create_id(Index index, Version version);
    static bool valid(ID id);
    static Index get_index(ID id);
    static Version get_version(ID id);

    ID id;
    Component::Mask mask;
};

constexpr uint16_t MAX_ENTITIES = 4096u;

};

#endif // BRASSTACKS_ECS_ENTITY_HPP