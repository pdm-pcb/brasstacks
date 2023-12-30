#include "brasstacks/config/RenderConfig.hpp"

namespace btx {

RenderConfig::SurfaceDimensions RenderConfig::target_window_size     = { };
RenderConfig::SurfacePosition   RenderConfig::target_window_position = { };

RenderConfig::SurfaceDimensions RenderConfig::swapchain_image_size   = { };
RenderConfig::SurfacePosition   RenderConfig::swapchain_image_offset = { };

uint32_t RenderConfig::swapchain_image_count = 0u;
float   RenderConfig::swapchain_aspect_ratio = 0.0f;

bool RenderConfig::vsync_on = true;

btx::Color RenderConfig::clear_color { 0.04f, 0.04f, 0.08f };

float RenderConfig::anisotropy = 1.0f;

uint32_t RenderConfig::msaa_samples = 1u;

} // namespace btx