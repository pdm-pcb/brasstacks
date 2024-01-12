#ifndef BRASSTACKS_CONFIG_RENDERCONFIG_HPP
#define BRASSTACKS_CONFIG_RENDERCONFIG_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/math/colors.hpp"

namespace btx {

struct RenderConfig final {

    struct Size { uint32_t width = 0u; uint32_t height = 0u; };
    struct Offset { int32_t x = 0u; int32_t y = 0u; };

    static bool vsync_on;

    static Color clear_color;

    static float anisotropy;

    static uint32_t msaa_samples;
};

} // namespace btx

#endif // BRASSTACKS_CONFIG_RENDERCONFIG_HPP