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
        vk::PhysicalDeviceFeatures2 const &features,
        std::span<char const * const> const extensions);

    static void clear_device_list();
    static void set_msaa_levels();
    static void set_aniso_levels();

    explicit vkPhysicalDevice(vk::PhysicalDevice const &handle);
    ~vkPhysicalDevice() = default;

    vkPhysicalDevice() = delete;

    inline auto native() const { return _handle; }
    inline std::string_view const name() const { return _name; }
    inline auto type() const { return _type; }
    inline auto vram_bytes() const { return _vram_bytes; }
    inline auto queue_family_index() const { return _queue_family_index; }
    inline auto & enabled_features() { return _enabled_features; }
    inline auto enabled_extensions() const { return _enabled_extensions; }

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

    vk::PhysicalDeviceFeatures2          _enabled_features;
    vk::PhysicalDeviceVulkan11Features   _enabled_features11;
    vk::PhysicalDeviceVulkan12Features   _enabled_features12;
    std::vector<vk::ExtensionProperties> _enabled_extensions;

    vk::SampleCountFlags _samples;
    float _max_aniso;

    static void _sort_device_list();

    static void _print_family_flags(uint32_t const family,
                                    vk::QueueFlags const flags);

    bool _check_queue_families(vkSurface const &surface);
    bool _check_features(vk::PhysicalDeviceFeatures2 const &features);
    bool _check_extensions(std::span<char const * const> extensions);
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKPHYSICALDEVICE_HPP