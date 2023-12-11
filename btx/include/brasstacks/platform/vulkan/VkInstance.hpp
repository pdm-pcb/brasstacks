#ifndef BRASSTACKS_PLATFORM_VULKAN_VKINSTANCE_HPP
#define BRASSTACKS_PLATFORM_VULKAN_VKINSTANCE_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class VkInstance final {
public:
    static void init();
    static void shutdown();

    inline static auto const& native() { return _instance; }

    VkInstance() = default;
    ~VkInstance() = default;

    VkInstance(VkInstance &&other) = delete;
    VkInstance(const VkInstance &other) = delete;

    VkInstance& operator=(VkInstance &&other) = delete;
    VkInstance& operator=(const VkInstance &other) = delete;

private:
    static vk::DynamicLoader _loader;
    static vk::Instance      _instance;

    static vk::ApplicationInfo       _app_info;
    static std::vector<char const *> _enabled_layers;
    static std::vector<char const *> _enabled_extensions;

    using ValidationFeatures = std::vector<vk::ValidationFeatureEnableEXT>;
    static ValidationFeatures _validation_features;

    static vk::ValidationFeaturesEXT _validation_extensions;

    static vk::InstanceCreateInfo _instance_create_info;

    static void _init_dynamic_loader();
    static void _init_app_info();
    static void _init_layers();
    static void _init_extensions();

    using Extensions = std::vector<vk::ExtensionProperties>;
    static bool _check_extensions(Extensions const &supported_extensions);
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_VKINSTANCE_HPP