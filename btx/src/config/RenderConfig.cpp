#include "brasstacks/brasstacks.hpp"
#include "brasstacks/config/RenderConfig.hpp"

namespace btx {

std::uint32_t RenderConfig::screen_width  = 0u;
std::uint32_t RenderConfig::screen_height = 0u;
std::int32_t  RenderConfig::screen_x_offset = 0;
std::int32_t  RenderConfig::screen_y_offset = 0;

std::uint32_t RenderConfig::window_width  = 0u;
std::uint32_t RenderConfig::window_height = 0u;
std::int32_t  RenderConfig::window_pos_x  = 0;
std::int32_t  RenderConfig::window_pos_y  = 0;
float RenderConfig::window_aspect = 0.0f;

std::uint32_t RenderConfig::swapchain_image_count = 2u;
bool RenderConfig::vsync_on = true;

std::uint32_t RenderConfig::max_msaa_samples = 1u;
float RenderConfig::anisotropy = 1.0f;

std::array<float, 4> RenderConfig::clear_color { 0.04f, 0.04f, 0.08f, 1.0f };

} // namespace btx