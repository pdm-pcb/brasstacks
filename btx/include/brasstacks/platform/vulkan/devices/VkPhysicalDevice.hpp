#ifndef BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKPHYSICALDEVICE_HPP
#define BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKPHYSICALDEVICE_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class VkPhysicalDevice final {
public:
    enum class Features {
        SAMPLER_ANISOTROPY,
        FILL_MODE_NONSOLID,
    };

    static void query_devices(
        std::vector<std::string_view> const & required_extensions,
        std::vector<Features> const & required_features
    );
    static void select_device();

    inline static auto queue_index()          { return _queue_index;        }
    inline static auto const & native()       { return _physical_device;    }
    inline static auto const & memory_props() { return _memory_properties;  }
    inline static auto const & features()     { return _enabled_features;   }
    inline static auto const & extensions()   { return _enabled_extensions; }

    VkPhysicalDevice() = delete;

private:
    struct DeviceProps {
        std::string name;
        size_t      vram_bytes = 0;
        uint8_t     max_samples = 0u;
        float       max_aniso = 0.0f;
        std::string driver_version;
        std::string vkapi_version;
        vk::PhysicalDevice device = nullptr;
        vk::PhysicalDeviceType type = vk::PhysicalDeviceType::eOther;
        vk::PhysicalDeviceMemoryProperties memory { };
    };
    using DeviceList = std::vector<DeviceProps>;
    static DeviceList _available_devices;

    static vk::PhysicalDevice _physical_device;

    static uint32_t _queue_index;

    static vk::PhysicalDeviceMemoryProperties _memory_properties;
    static vk::PhysicalDeviceFeatures         _enabled_features;
    static std::vector<char const *>          _enabled_extensions;

    static bool _check_features(
        vk::PhysicalDeviceFeatures const &supported_features,
        std::vector<Features> const &required_features
    );

    static bool _check_extensions(
        std::vector<vk::ExtensionProperties> const &supported_extensions,
        std::vector<std::string_view> const &required_extensions
    );

    static void _store_physical_device(
        const vk::PhysicalDevice &device,
        const vk::PhysicalDeviceProperties &properties,
        const vk::PhysicalDeviceMemoryProperties &memory,
        const vk::PhysicalDeviceDriverProperties &drivers
    );

    static void _print_family_flags(uint32_t const family,
                                    const vk::QueueFlags flags);
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKPHYSICALDEVICE_HPP