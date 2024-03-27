#include "brasstacks/config/RenderConfig.hpp"

namespace btx {

bool RenderConfig::vsync_on = true;

float RenderConfig::anisotropy = 1.0f;

uint8_t RenderConfig::msaa_samples = 1u;

uint8_t RenderConfig::display_bit_depth = 24u;

} // namespace btx