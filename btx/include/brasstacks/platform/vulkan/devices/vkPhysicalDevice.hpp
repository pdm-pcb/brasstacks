/**
 * @file vkPhysicalDevice.hpp
 * @brief A wrapper class for Vulkan's idea of a physical GPU.
 */

#ifndef BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKPHYSICALDEVICE_HPP
#define BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKPHYSICALDEVICE_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkInstance;
class vkSurface;

/**
 * @brief A wrapper class for Vulkan's idea of a physical GPU.
 *
 * This class is a glorified collection of details. It enumerates the GPUs the
 * Vulkan instance can find, then sorts them with a preference for dGPUs and
 * VRAM. It then goes through the list of available devices and chooses the
 * first one it comes across that satisfy the following criteria:
 *
 *  - Support for graphics and present commands in the same queue family
 *  - Support for all required features
 *  - Support for all required extensions
 *
 * Once the selection process is complete, the properties of the chosen GPU are
 * saved for later systems to query.
 */
class vkPhysicalDevice final {
public:
    /**
     * @brief A means by which to request physical device features.
     */
    enum class Features {
        SAMPLER_ANISOTROPY,
        FILL_MODE_NONSOLID,
    };
    using FeatureList = std::vector<Features>;

    /**
     * @brief A means by which to request physical device extensions.
     */
    using ExtensionList = std::vector<std::string_view>;

    /**
     * @brief Construct the vk Physical Device object.
     * @param instance An established Vulkan instance
     * @param surface An established Vulkan surface
     * @param required_features A list of features the selected device must
     * support
     * @param required_extensions A list of extensions the selected device must
     * support
     */
    vkPhysicalDevice(vkInstance    const &instance,
                     vkSurface     const &surface,
                     FeatureList   const &required_features,
                     ExtensionList const &required_extensions);

    ~vkPhysicalDevice() = default;

    /**
     * @brief Return the device queue family index.
     * @return uint32_t
     */
    inline auto graphics_queue_index() const {
        return _chosen_device.graphics_queue_index;
    }

    /**
     * @brief Return a list of enabled device features.
     * @return struct vk::PhysicalDeviceFeatures const&
     */
    inline auto const & features() const {
        return _chosen_device.enabled_features;
    }

    /**
     * @brief Return a list of enabled device features.
     * @return struct vk::PhysicalDeviceFeatures const&
     */
    inline auto const & extensions() const {
        return _chosen_device.enabled_extensions;
    }

    /**
     * @brief Return the native Vulkan handle.
     * @return vk::PhysicalDevice const&
     */
    inline auto const & native() const { return _chosen_device.handle; }

    /**
     * @brief Return the details of the chosen device's memory
     * @return vk::PhysicalDeviceMemoryProperties const&
     */
    inline auto const & memory_props() const { return _chosen_device.memory; }

    vkPhysicalDevice() = delete;

    vkPhysicalDevice(vkPhysicalDevice &&) = delete;
    vkPhysicalDevice(const vkPhysicalDevice &) = delete;

    vkPhysicalDevice & operator=(vkPhysicalDevice &&) = delete;
    vkPhysicalDevice & operator=(const vkPhysicalDevice &) = delete;

private:
    /**
     * @brief Details of a given device
     */
    struct DeviceProps {
        std::string name;
        std::string driver_version;
        std::string vkapi_version;

        vk::DeviceSize vram_bytes = 0;
        uint32_t graphics_queue_index = std::numeric_limits<uint32_t>::max();

        vk::PhysicalDevice handle = nullptr;
        vk::PhysicalDeviceType type = vk::PhysicalDeviceType::eOther;

        vk::PhysicalDeviceMemoryProperties memory { };

        vk::PhysicalDeviceFeatures enabled_features;
        std::vector<char const *> enabled_extensions;
    };

    /**
     * @brief A list of devices to choose from
     */
    std::vector<DeviceProps> _available_devices;

    /**
     * @brief The details of the device that supported provided requirements
     */
    DeviceProps _chosen_device;

    /**
     * @brief Make a list of phsyical devices available via this instance
     * @param instance An established Vulkan instance
     */
    void _enumerate_and_sort(vkInstance const &instance);

    /**
     * @brief Check if a device supports graphics and present commands within
     * a single queue family
     * @param device The device to check
     * @param surface The surface is required to check for presentat support
     * @return true If the device does support graphics and present commands
     * within a single queue family
     * @return false If the device does not support graphics and present
     * commands within a single queue family
     */
    bool _check_queue_families(DeviceProps &device, vkSurface const &surface);

    /**
     * @brief Check if a device supports a given list of features
     * @param device The device to check
     * @param required_features The features the device must support
     * @return true If the device supports all of the features
     * @return false If the device does not support all of the features
     */
    bool _check_features(DeviceProps &device,
                         FeatureList const &required_features);

    /**
     * @brief Check if a device supports a given list of extensions
     * @param device The device to check
     * @param required_extensions The extensions the device must support
     * @return true If the device supports all of the extensions
     * @return false If the device does not support all of the extensions
     */
    bool _check_extensions(DeviceProps &device,
                           ExtensionList const &required_extensions);

    /**
     * @brief Populate a DeviceProps struct and add it to _available_devices
     * @param device The native Vulkan device handle
     */
    void _store_device(vk::PhysicalDevice const &device);

    /**
     * @brief Log queue family capabilities
     * @param family Queue family index
     * @param flags Flags indicating this queue family's abilites
     */
    void _print_family_flags(uint32_t const family, vk::QueueFlags const flags);
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKPHYSICALDEVICE_HPP