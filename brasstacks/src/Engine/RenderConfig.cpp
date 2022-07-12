#include "brasstacks/Engine/RenderConfig.hpp"

namespace btx {

size_t RenderConfig::adapter = 0;
size_t RenderConfig::display = 0;

int16_t RenderConfig::window_x_res = 0;
int16_t RenderConfig::window_y_res = 0;
int16_t RenderConfig::refresh_rate = 0;

bool RenderConfig::vsync_on   = true;
bool RenderConfig::fullscreen = false;

size_t RenderConfig::selected_api = 0;
std::vector<const char *> RenderConfig::supported_apis {
    "OpenGL",
    "Vulkan",
    "DirectX 11",
    "DirectX 12"
};

} // namespace btx