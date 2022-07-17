#ifndef BRASSTACKS_ECS_ENTITY_HPP
#define BRASSTACKS_ECS_ENTITY_HPP

#include <cstdint>

namespace btx {

using Entity = std::uint32_t;
constexpr Entity MAX_ENTITIES = 5120;

};

#endif // BRASSTACKS_ECS_ENTITY_HPP