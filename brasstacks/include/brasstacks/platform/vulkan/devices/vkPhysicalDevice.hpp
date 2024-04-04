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
        FILL_MODE_NONSOLID,
        SAMPLER_ANISOTROPY,
    };
    using FeatureList = std::vector<Features>;

    /**
     * @brief A means by which to request physical device extensions.
     */
    using ExtensionList = std::vector<std::string_view>;

    /**
     * @brief Selects a physical device based on surface capabilities, required
     * features, and required extensions
     * @param surface An established Vulkan surface
     * @param required_features A list of features the selected device must
     * support
     * @param required_extensions A list of extensions the selected device must
     * support
     */
    static void select(vkSurface     const &surface,
                       FeatureList   const &required_features,
                       ExtensionList const &required_extensions);

    vkPhysicalDevice() = delete;
    ~vkPhysicalDevice() = delete;

    vkPhysicalDevice(vkPhysicalDevice &&) = delete;
    vkPhysicalDevice(const vkPhysicalDevice &) = delete;

    vkPhysicalDevice & operator=(vkPhysicalDevice &&) = delete;
    vkPhysicalDevice & operator=(const vkPhysicalDevice &) = delete;

private:
    /**
     * @brief Make a list of phsyical devices available via this instance
     * @param instance An established Vulkan instance
     */
    static void _enumerate_and_sort(vk::Instance const &instance);

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
    static bool _check_queue_families(RenderConfig::DeviceProps &device,
                                      vkSurface const &surface);

    /**
     * @brief Check if a device supports a given list of features
     * @param device The device to check
     * @param required_features The features the device must support
     * @return true If the device supports all of the features
     * @return false If the device does not support all of the features
     */
    static bool _check_features(RenderConfig::DeviceProps &device,
                                FeatureList const &required_features);

    /**
     * @brief Check if a device supports a given list of extensions
     * @param device The device to check
     * @param required_extensions The extensions the device must support
     * @return true If the device supports all of the extensions
     * @return false If the device does not support all of the extensions
     */
    static bool _check_extensions(RenderConfig::DeviceProps &device,
                                  ExtensionList const &required_extensions);

    /**
     * @brief Populate a DeviceProps struct and add it to _available_devices
     * @param device The native Vulkan device handle
     */
    static void _store_device(vk::PhysicalDevice const &device);

    /**
     * @brief Log queue family capabilities
     * @param family Queue family index
     * @param flags Flags indicating this queue family's abilites
     */
    static void _print_family_flags(uint32_t const family,
                                    vk::QueueFlags const flags);

    static void _get_msaa_levels();
    static void _get_aniso_levels();
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKPHYSICALDEVICE_HPP