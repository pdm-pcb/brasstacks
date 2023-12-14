#ifndef BRASSTACKS_CONFIG_RENDERCONFIG_HPP
#define BRASSTACKS_CONFIG_RENDERCONFIG_HPP

#include "brasstacks/pch.hpp"

namespace btx {

struct RenderConfig final {
    static bool vsync_on;

    static std::array<float, 4> clear_color;
};

} // namespace btx

#endif // BRASSTACKS_CONFIG_RENDERCONFIG_HPP