/**
 * @file vkInstance.hpp
 * @brief A wrapper class for a Vulkan instance.
 */

#ifndef BRASSTACKS_PLATFORM_VULKAN_VKINSTANCE_HPP
#define BRASSTACKS_PLATFORM_VULKAN_VKINSTANCE_HPP

#include "brasstacks/pch.hpp"

namespace btx {

/**
 * @brief A wrapper class for a Vulkan instance.
 *
 * This class manages creation of the Vulkan instance, initial setup of the
 * dynamic loader provided by VulkanHpp, and validating/enabling any requested
 * instance extensions.
 */
class vkInstance final {
public:
    /**
     * @brief Creates the vkInstance object
     * @param api_version The version of the Vulkan API to request from the
     * driver. Defaults to Vulkan 1.2.
     */
    static void create(uint32_t const api_version = VK_API_VERSION_1_2);
    static void destroy();

    /**
     * @brief Return the native Vulkan instance handle
     * @return vk::Instance const &
     */
    static inline auto const & native() { return _handle; }

    vkInstance() = delete;
    ~vkInstance() = delete;

    vkInstance(vkInstance &&other) = delete;
    vkInstance(const vkInstance &other) = delete;

    vkInstance& operator=(vkInstance &&other) = delete;
    vkInstance& operator=(const vkInstance &other) = delete;

private:
    /**
     * @brief Allows the application to call Vulkan functions without first
     * loading the function pointers manually.
     */
    static vk::DynamicLoader _loader;

    /**
     * @brief Which version of the API we want to use.
     */
    static uint32_t _target_api_version;

    /**
     * @brief Hints provided to the driver like API version, engine/application
     * name, etc.
     */
    static vk::ApplicationInfo _app_info;

    /**
     * @brief The collection of layers being requested. Typically, the
     * validation layer will be requested in debug and none will be requested
     * in release.
     */
    static std::vector<char const *> _enabled_layers;

    /**
     * @brief The collection of instance extensions being requested.
     */
    static std::vector<char const *> _enabled_extensions;

#ifdef BTX_DEBUG
    using ValidationFeatures = std::vector<vk::ValidationFeatureEnableEXT>;
    static ValidationFeatures _validation_features;
    static vk::ValidationFeaturesEXT _validation_extensions;
#endif // BTX_DEBUG

    /**
     * @brief Native Vulkan instance handle.
     */
    static vk::Instance _handle;

    static void _init_dynamic_loader();
    static void _init_app_info();
    static void _init_layers();
    static void _init_extensions();

    using Extensions = std::vector<vk::ExtensionProperties>;
    static bool _check_extensions(Extensions const &supported_extensions);
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_VKINSTANCE_HPP