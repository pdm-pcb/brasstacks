#include "brasstacks/config/RenderConfig.hpp"

namespace btx {

bool RenderConfig::vsync_on = true;

btx::Color RenderConfig::clear_color { 0.04f, 0.04f, 0.08f };

float RenderConfig::anisotropy = 1.0f;

uint32_t RenderConfig::msaa_samples = 1u;

} // namespace btx