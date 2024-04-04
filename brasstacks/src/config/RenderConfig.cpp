#include "brasstacks/config/RenderConfig.hpp"

namespace btx {

RenderConfig::DeviceProps *RenderConfig::current_device { nullptr };
std::vector<RenderConfig::DeviceProps> RenderConfig::available_devices { };

std::array<RenderConfig::SelectedResolution, 4>
    RenderConfig::available_resolutions { };

RenderConfig::SelectedResolution *RenderConfig::current_resolution { nullptr };

uint8_t RenderConfig::current_msaa { 2u };
std::vector<uint8_t> RenderConfig::available_msaa { };

uint8_t RenderConfig::current_aniso { 1u };
std::vector<uint8_t> RenderConfig::available_aniso { };

bool RenderConfig::vsync_on { true };

} // namespace btx