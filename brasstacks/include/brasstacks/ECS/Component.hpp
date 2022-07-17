#ifndef BRASSTACKS_ECS_COMPONENT_HPP
#define BRASSTACKS_ECS_COMPONENT_HPP

#include <cstdint>
#include <bitset>

namespace btx {

using ComponentType = std::uint8_t;
constexpr ComponentType MAX_COMPONENTS = 128;

using Signature = std::bitset<MAX_COMPONENTS>;

} // namespace btx

#endif // BRASSTACKS_ECS_COMPONENT_HPP