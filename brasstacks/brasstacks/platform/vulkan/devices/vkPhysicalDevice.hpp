/**
 * @file vkPhysicalDevice.hpp
 * @brief A wrapper class for Vulkan's idea of a physical GPU.
 */

#ifndef BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKPHYSICALDEVICE_HPP
#define BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKPHYSICALDEVICE_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/config/RenderConfig.hpp"

namespace btx {

class vkSurface;

class vkPhysicalDevice final {
public:
    static void populate_device_list(
        vkSurface const &surface,
        vk::PhysicalDeviceFeatures const &features,
        std::span<char const * const> const extensions);

    static void clear_device_list();
    static void set_msaa_levels();
    static void set_aniso_levels();

    explicit vkPhysicalDevice(vk::PhysicalDevice const &handle);
    ~vkPhysicalDevice() = default;

    vkPhysicalDevice() = delete;

    bool check_queue_families(vkSurface const &surface);
    bool check_features(vk::PhysicalDeviceFeatures const &features);
    bool check_extensions(std::span<char const * const> extensions);

    auto native() const { return _handle; }
    std::string_view const name() const { return _name; }
    auto type() const { return _type; }
    auto vram_bytes() const { return _vram_bytes; }
    auto queue_family_index() const { return _queue_family_index; }
    auto const & enabled_features() const { return _enabled_features; }
    std::span<vk::ExtensionProperties const> const enabled_extensions() const {
        return _enabled_extensions;
    }

    auto const & samples() const { return _samples; }
    auto max_aniso() const { return _max_aniso; }

    vkPhysicalDevice(vkPhysicalDevice &&) = delete;
    vkPhysicalDevice(const vkPhysicalDevice &) = delete;

    vkPhysicalDevice & operator=(vkPhysicalDevice &&) = delete;
    vkPhysicalDevice & operator=(const vkPhysicalDevice &) = delete;

private:
    vk::PhysicalDevice _handle;
    vk::PhysicalDeviceType _type;

    std::string _name;
    std::string _vkapi_version;
    size_t      _vram_bytes;
    std::string _driver_version;

    uint32_t    _queue_family_index;

    vk::PhysicalDeviceFeatures           _enabled_features;
    std::vector<vk::ExtensionProperties> _enabled_extensions;

    vk::SampleCountFlags _samples;
    float _max_aniso;

    static void _sort_device_list();

    static void _print_family_flags(uint32_t const family,
                                    vk::QueueFlags const flags);
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKPHYSICALDEVICE_HPP