#ifndef BRASSTACKS_PLATFORM_VULKAN_VKINSTANCE_HPP
#define BRASSTACKS_PLATFORM_VULKAN_VKINSTANCE_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkInstance final {
public:
    struct Config {
        std::initializer_list<char const *> extensions { };
        bool enable_validation = false;
    };

    static bool create(Config const &config,
                       std::string_view const app_name,
                       uint32_t app_version);
    static bool destroy();

    static inline auto const & native() { return _handle; }
    static inline auto const & loader() { return _loader; }

    vkInstance() = delete;
    ~vkInstance() = delete;

    vkInstance(vkInstance &&other) = delete;
    vkInstance(const vkInstance &other) = delete;

    vkInstance& operator=(vkInstance &&other) = delete;
    vkInstance& operator=(const vkInstance &other) = delete;

private:
    static vk::Instance _handle;

    static std::string _app_name;
    static std::uint32_t _app_version;

    static vk::DynamicLoader         _loader;
    static vk::ApplicationInfo       _app_info;
    static std::vector<char const *> _enabled_layers;
    static std::vector<char const *> _enabled_extensions;

    static std::vector<vk::ValidationFeatureEnableEXT>  _vvl_enabled;
    static std::vector<vk::ValidationFeatureDisableEXT> _vvl_disabled;
    static vk::ValidationFeaturesEXT                    _vvl_features;

    static void _init_dynamic_loader();
    static void _init_app_info();
    static void _init_validation();
    static bool _check_layers();
    static bool _check_extensions();
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_VKINSTANCE_HPP