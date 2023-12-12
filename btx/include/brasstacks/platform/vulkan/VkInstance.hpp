#ifndef BRASSTACKS_PLATFORM_VULKAN_VKINSTANCE_HPP
#define BRASSTACKS_PLATFORM_VULKAN_VKINSTANCE_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkInstance final {
public:
    inline auto const& native() const { return _handle; }

    explicit vkInstance();
    ~vkInstance();

    vkInstance(vkInstance &&other) = delete;
    vkInstance(const vkInstance &other) = delete;

    vkInstance& operator=(vkInstance &&other) = delete;
    vkInstance& operator=(const vkInstance &other) = delete;

private:
    vk::DynamicLoader         _loader;
    uint32_t const            _target_api_version;
    vk::ApplicationInfo       _app_info;
    std::vector<char const *> _enabled_layers;
    std::vector<char const *> _enabled_extensions;

    using ValidationFeatures = std::vector<vk::ValidationFeatureEnableEXT>;
    ValidationFeatures _validation_features;
    vk::ValidationFeaturesEXT _validation_extensions;

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