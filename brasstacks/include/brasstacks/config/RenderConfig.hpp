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

    static DeviceProps *current_device;
    static std::vector<DeviceProps> available_devices;

    static struct SelectedResolution *current_resolution;
    static std::array<SelectedResolution, 4> available_resolutions;

    static uint8_t current_msaa;
    static std::vector<uint8_t> available_msaa;

    static uint8_t current_aniso;
    static std::vector<uint8_t> available_aniso;

    static bool vsync_on;
};

} // namespace btx

#endif // BRASSTACKS_CONFIG_RENDERCONFIG_HPP