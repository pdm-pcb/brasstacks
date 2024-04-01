#include "brasstacks/config/RenderConfig.hpp"

namespace btx {

bool RenderConfig::vsync_on { true };

float RenderConfig::anisotropy { 1.0f };

uint8_t RenderConfig::msaa_samples { 1u };

std::array<RenderConfig::Size, 4> RenderConfig::resolutions { };

} // namespace btx