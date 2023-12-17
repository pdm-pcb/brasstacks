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
     * @brief Construct the vkInstance object.
     * @param api_version The version of the Vulkan API to request from the
     * driver. Defaults to Vulkan 1.2.
     */
    explicit vkInstance(uint32_t const api_version = VK_API_VERSION_1_2);
    ~vkInstance();

    /**
     * @brief Return the native Vulkan instance handle
     * @return vk::Instance const &
     */
    inline auto const & native() const { return _handle; }

    vkInstance(vkInstance &&other) = delete;
    vkInstance(const vkInstance &other) = delete;

    vkInstance& operator=(vkInstance &&other) = delete;
    vkInstance& operator=(const vkInstance &other) = delete;

private:
    /**
     * @brief Allows the application to call Vulkan functions without first
     * loading the function pointers manually.
     */
    vk::DynamicLoader _loader;

    /**
     * @brief Which version of the API we want to use.
     */
    uint32_t const _target_api_version;

    /**
     * @brief Hints provided to the driver like API version, engine/application
     * name, etc.
     */
    vk::ApplicationInfo _app_info;

    /**
     * @brief The collection of layers being requested. Typically, the
     * validation layer will be requested in debug and none will be requested
     * in release.
     */
    std::vector<char const *> _enabled_layers;

    /**
     * @brief The collection of instance extensions being requested.
     */
    std::vector<char const *> _enabled_extensions;

#ifdef BTX_DEBUG
    using ValidationFeatures = std::vector<vk::ValidationFeatureEnableEXT>;
    ValidationFeatures _validation_features;
    vk::ValidationFeaturesEXT _validation_extensions;
#endif // BTX_DEBUG

    /**
     * @brief Native Vulkan instance handle.
     */
    vk::Instance _handle;

    void _init_dynamic_loader();
    void _init_app_info();
    void _init_layers();
    void _init_extensions();

    using Extensions = std::vector<vk::ExtensionProperties>;
    bool _check_extensions(Extensions const &supported_extensions);
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_VKINSTANCE_HPP