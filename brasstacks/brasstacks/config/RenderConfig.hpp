#ifndef BRASSTACKS_CONFIG_RENDERCONFIG_HPP
#define BRASSTACKS_CONFIG_RENDERCONFIG_HPP

#include "brasstacks/pch.hpp"

namespace btx {

struct RenderConfig final {
    struct Size final {
        uint32_t width = 0u;
        uint32_t height = 0u;
    };

    struct Offset final {
        int32_t x = 0u;
        int32_t y = 0u;
    };

    struct DeviceProps final {
        vk::PhysicalDevice handle { nullptr };
        vk::PhysicalDeviceMemoryProperties memory { };
        vk::PhysicalDeviceType type = vk::PhysicalDeviceType::eOther;

        vk::DeviceSize vram_bytes { 0u };

        std::string name { };
        std::string driver_version { };
        std::string vkapi_version { };

        vk::SampleCountFlags max_samples = vk::SampleCountFlagBits::e1;
        float max_aniso { 0.0f };

        uint32_t graphics_queue_index = std::numeric_limits<uint32_t>::max();

        vk::PhysicalDeviceFeatures enabled_features { };
        std::vector<char const *> enabled_extensions { };

        bool selected = false;
    };

    struct SelectedResolution final {
        Size size = { };
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

    static std::vector<DeviceProps> available_devices;
    static DeviceProps *current_device;

    static std::array<SelectedResolution, 4> available_resolutions;
    static struct SelectedResolution *current_resolution;

    static std::vector<SelectedMSAA> available_msaa;
    static struct SelectedMSAA *current_msaa;

    static std::vector<SelectedAniso> available_aniso;
    static struct SelectedAniso *current_aniso;

    static bool vsync_on;
};

} // namespace btx

#endif // BRASSTACKS_CONFIG_RENDERCONFIG_HPP