#ifndef BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKPHYSICALDEVICE_HPP
#define BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKPHYSICALDEVICE_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkInstance;

class vkPhysicalDevice final {
public:
    enum class Features {
        SAMPLER_ANISOTROPY,
        FILL_MODE_NONSOLID,
    };

    using ExtensionList = std::vector<std::string_view>;
    using FeatureList = std::vector<Features>;

    inline auto queue_index()          const { return _queue_index;        }
    inline auto const & native()       const { return _handle;             }
    inline auto const & memory_props() const { return _memory_properties;  }
    inline auto const & features()     const { return _enabled_features;   }
    inline auto const & extensions()   const { return _enabled_extensions; }

    vkPhysicalDevice(vkInstance const &instance,
                     ExtensionList const &required_extensions,
                     FeatureList const &required_features,
                     bool const order_by_perf = false);
    ~vkPhysicalDevice() = default;

    vkPhysicalDevice() = delete;

    vkPhysicalDevice(vkPhysicalDevice &&) = delete;
    vkPhysicalDevice(const vkPhysicalDevice &) = delete;

    vkPhysicalDevice & operator=(vkPhysicalDevice &&) = delete;
    vkPhysicalDevice & operator=(const vkPhysicalDevice &) = delete;

private:
    struct DeviceProps {
        std::string name;
        size_t  vram_bytes = 0;
        uint8_t max_samples = 0u;
        float   max_aniso = 0.0f;
        std::string driver_version;
        std::string vkapi_version;
        vk::PhysicalDevice device = nullptr;
        vk::PhysicalDeviceType type = vk::PhysicalDeviceType::eOther;
        vk::PhysicalDeviceMemoryProperties memory { };
    };

    using DeviceList = std::vector<DeviceProps>;
    DeviceList _available_devices;

    vk::PhysicalDevice _handle;

    uint32_t _queue_index;

    vk::PhysicalDeviceMemoryProperties _memory_properties;
    vk::PhysicalDeviceFeatures         _enabled_features;
    std::vector<char const *>          _enabled_extensions;

    vkInstance const &_instance;

    void _select_device();

    bool _check_features(
        vk::PhysicalDeviceFeatures const &supported_features,
        FeatureList const &required_features
    );

    bool _check_extensions(
        std::vector<vk::ExtensionProperties> const &supported_extensions,
        ExtensionList const &required_extensions
    );

    void _store_device(
        const vk::PhysicalDevice &device,
        const vk::PhysicalDeviceProperties &properties,
        const vk::PhysicalDeviceMemoryProperties &memory,
        const vk::PhysicalDeviceDriverProperties &drivers
    );

    void _print_family_flags(uint32_t const family,
                                    const vk::QueueFlags flags);
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKPHYSICALDEVICE_HPP