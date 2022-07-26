#include "brasstacks/System/pch.hpp"
#include "brasstacks/Engine/RenderConfig.hpp"

namespace btx {

void RenderConfig::enable_blending() {
#ifdef BTX_OPENGL
    ::glEnable(GL_BLEND);
    ::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#elif defined(BTX_DIRECTX11)
    // ...?
#endif
}

void RenderConfig::disable_blending() {
#ifdef BTX_OPENGL
    ::glDisable(GL_BLEND);
#elif defined(BTX_DIRECTX11)
    // ...?
#endif
}

std::size_t RenderConfig::adapter = 0;
std::size_t RenderConfig::display = 0;

int16_t RenderConfig::window_x_res = 0;
int16_t RenderConfig::window_y_res = 0;
int16_t RenderConfig::refresh_rate = 0;
float   RenderConfig::aspect_ratio = 16.0f / 9.0f;

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