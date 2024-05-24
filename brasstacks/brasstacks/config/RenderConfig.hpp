#ifndef BRASSTACKS_CONFIG_RENDERCONFIG_HPP
#define BRASSTACKS_CONFIG_RENDERCONFIG_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkPhysicalDevice;

struct RenderConfig final {
    struct Size final {
        uint32_t width = 0u;
        uint32_t height = 0u;
    };

    struct Offset final {
        int32_t x = 0u;
        int32_t y = 0u;
    };

    struct SelectedDevice final {
        vkPhysicalDevice *device;
        bool selected = false;
    };

    struct SelectedResolution final {
        Size size = { };
        // uint32_t refresh_rate = 0;
        bool selected = false;
    };

    struct SelectedMSAA final {
        uint8_t msaa = 0u;
        bool selected = false;
    };

    struct SelectedAniso final {
        uint8_t aniso = 0u;
        bool selected = false;
    };

    static std::vector<SelectedDevice> available_devices;
    static SelectedDevice *current_device;

    static std::vector<SelectedResolution> available_resolutions;
    static struct SelectedResolution *current_resolution;

    static std::vector<SelectedMSAA> available_msaa;
    static struct SelectedMSAA *current_msaa;

    static std::vector<SelectedAniso> available_aniso;
    static struct SelectedAniso *current_aniso;

    static bool vsync_on;
};

} // namespace btx

#endif // BRASSTACKS_CONFIG_RENDERCONFIG_HPP