#ifndef BRASSTACKS_CONFIG_RENDERCONFIG_HPP
#define BRASSTACKS_CONFIG_RENDERCONFIG_HPP

#include "brasstacks/pch.hpp"

namespace btx {

struct RenderConfig final {

    struct Size { uint32_t width = 0u; uint32_t height = 0u; };
    struct Offset { int32_t x = 0u; int32_t y = 0u; };

    struct SelectedResolution { Size size = { }; bool selected = false; };

    static bool vsync_on;

    static float anisotropy;

    static uint8_t msaa_samples;

    static struct SelectedResolution *current_resolution;
    static std::array<SelectedResolution, 4> resolutions;
};

} // namespace btx

#endif // BRASSTACKS_CONFIG_RENDERCONFIG_HPP