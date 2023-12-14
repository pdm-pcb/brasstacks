#include "brasstacks/config/RenderConfig.hpp"

namespace btx {

bool RenderConfig::vsync_on = true;

std::array<float, 4> RenderConfig::clear_color { 0.04f, 0.04f, 0.08f, 1.0f };

} // namespace btx