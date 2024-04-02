#include "brasstacks/config/RenderConfig.hpp"

namespace btx {

bool RenderConfig::vsync_on { true };

uint8_t RenderConfig::current_aniso { 1u };
std::vector<uint8_t> RenderConfig::available_aniso { };

uint8_t RenderConfig::current_msaa { 2u };
std::vector<uint8_t> RenderConfig::available_msaa { };

RenderConfig::SelectedResolution *RenderConfig::current_resolution { nullptr };

std::array<RenderConfig::SelectedResolution, 4>
    RenderConfig::available_resolutions { };

} // namespace btx