#ifndef BRASSTACKS_ENGINE_RENDERCONFIG_HPP
#define BRASSTACKS_ENGINE_RENDERCONFIG_HPP

#include <cstdint>
#include <vector>

namespace btx {

struct RenderConfig {
    static std::size_t adapter;
    static std::size_t display; // TODO: support display picking

    static int16_t window_x_res;
    static int16_t window_y_res;
    static int16_t refresh_rate;

    static bool vsync_on;
    static bool fullscreen;

    static std::size_t selected_api;
    static std::vector<const char *> supported_apis;
};

} // namespace btx

#endif // BRASSTACKS_ENGINE_RENDERCONFIG_HPP