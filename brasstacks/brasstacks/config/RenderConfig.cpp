#include "brasstacks/config/RenderConfig.hpp"

namespace btx {

std::vector<RenderConfig::SelectedDevice> RenderConfig::available_devices { };
RenderConfig::SelectedDevice *RenderConfig::current_device { nullptr };

std::vector<RenderConfig::SelectedResolution>
    RenderConfig::available_resolutions { };

RenderConfig::SelectedResolution *RenderConfig::current_resolution { nullptr };

std::vector<RenderConfig::SelectedMSAA> RenderConfig::available_msaa { };
RenderConfig::SelectedMSAA *RenderConfig::current_msaa { nullptr };

std::vector<RenderConfig::SelectedAniso> RenderConfig::available_aniso { };
RenderConfig::SelectedAniso *RenderConfig::current_aniso { nullptr };

bool RenderConfig::vsync_on { true };

} // namespace btx