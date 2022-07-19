#include "brasstacks/Engine/RenderConfig.hpp"

namespace btx {

std::size_t RenderConfig::adapter = 0;
std::size_t RenderConfig::display = 0;

int16_t RenderConfig::window_x_res = 0;
int16_t RenderConfig::window_y_res = 0;
int16_t RenderConfig::refresh_rate = 0;

// TODO: Make these configurable as well?
float RenderConfig::near_clip = 0.1f;
float RenderConfig::far_clip  = 5000.0f;

bool RenderConfig::vsync_on   = true;
bool RenderConfig::fullscreen = false;

std::size_t RenderConfig::selected_api = 0;
std::vector<const char *> RenderConfig::supported_apis {
    "OpenGL",
    "Vulkan",
    "DirectX 11",
    "DirectX 12"
};

} // namespace btx