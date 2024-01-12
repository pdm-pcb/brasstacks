#ifndef BRASSTACKS_CONFIG_RENDERCONFIG_HPP
#define BRASSTACKS_CONFIG_RENDERCONFIG_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/math/colors.hpp"

namespace btx {

struct RenderConfig final {
    struct Dimensions final {
        uint32_t width = 0;
        uint32_t height = 0;
    };

    struct Offset final {
        int32_t x = 0;
        int32_t y = 0;
    };

    static Dimensions target_window_size;
    static Offset     target_window_position;

    static Dimensions swapchain_image_size;
    static Offset     swapchain_image_offset;
    static uint32_t   swapchain_image_count;
    static float      swapchain_aspect_ratio;

    static bool vsync_on;

    static Color clear_color;

    static float anisotropy;

    static uint32_t msaa_samples;
};

} // namespace btx

#endif // BRASSTACKS_CONFIG_RENDERCONFIG_HPP