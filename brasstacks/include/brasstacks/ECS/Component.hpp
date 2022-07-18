#ifndef BRASSTACKS_ECS_COMPONENT_HPP
#define BRASSTACKS_ECS_COMPONENT_HPP

#include <cstdint>
#include <bitset>

namespace btx {

constexpr uint8_t MAX_COMPONENTS = 32u;

struct Component {
    static std::uint32_t counter;

    using ID   = std::uint32_t;
    using Mask = std::bitset<MAX_COMPONENTS>;

    template <class T>
    static ID get_id() {
        static ID id = counter++;
        return id;
    }
};

} // namespace btx

#endif // BRASSTACKS_ECS_COMPONENT_HPP