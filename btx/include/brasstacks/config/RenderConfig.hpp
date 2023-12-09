#ifndef BRASSTACKS_CONFIG_RENDERCONFIG_HPP
#define BRASSTACKS_CONFIG_RENDERCONFIG_HPP

#include "brasstacks/pch.hpp"

namespace btx {

struct RenderConfig final {
    static std::uint32_t screen_width;
    static std::uint32_t screen_height;
    static std::int32_t  screen_x_offset;
    static std::int32_t  screen_y_offset;

    static std::uint32_t window_width;
    static std::uint32_t window_height;
    static std::int32_t  window_pos_x;
    static std::int32_t  window_pos_y;
    static float window_aspect;

    static std::uint32_t swapchain_image_count;
    static bool vsync_on;

    static std::uint32_t max_msaa_samples;
    static float anisotropy;

    static std::array<float, 4> clear_color;

    static std::uint32_t max_directional_lights;
    static std::uint32_t max_point_lights;
    static std::uint32_t max_spot_lights;
};

} // namespace btx

#endif // BRASSTACKS_CONFIG_RENDERCONFIG_HPP