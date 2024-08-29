#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/vkInstance.hpp"

#include "brasstacks/platform/vulkan/vkDebugger.hpp"

// This (and more) does away with the explicit loading of each
// function/extension. See: https://github.com/KhronosGroup/Vulkan-Hpp
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace btx {

vk::Instance vkInstance::_handle { nullptr };

std::string vkInstance::_app_name { };
std::uint32_t vkInstance::_app_version = 0u;

vk::DynamicLoader         vkInstance::_loader { };
vk::ApplicationInfo       vkInstance::_app_info { };
std::vector<char const *> vkInstance::_enabled_layers;
std::vector<char const *> vkInstance::_enabled_extensions;

std::vector<vk::ValidationFeatureEnableEXT> vkInstance::_vvl_enabled;
std::vector<vk::ValidationFeatureDisableEXT> vkInstance::_vvl_disabled;
vk::ValidationFeaturesEXT vkInstance::_vvl_features { };

// =============================================================================
bool vkInstance::create(Config const &config,
                        std::string_view const app_name, uint32_t app_version)
{
    if(_handle) {
        BTX_CRITICAL("Vulkan instance {} already exists", _handle);
        return false;
    }

    _app_name = app_name;
    _app_version = app_version;

    _enabled_extensions = config.extensions;

    _init_dynamic_loader(); // The first step for using the dynamic loader
    _init_app_info();       // Provide hints about this app to the driver

    // If validation is requested, add the layers, extensions, and features
    if(config.enable_validation) {
        _init_validation();
    }

    // Run through the extensions the driver offers and make sure we've got
    // what we need
    if(!_check_layers()) {
        BTX_CRITICAL("Could not get support for all requested instance "
                     "layers.");
        return false;
    }

    // Run through the extensions the driver offers and make sure we've got
    // what we need
    if(!_check_extensions()) {
        BTX_CRITICAL("Could not get support for all requested instance "
                     "extensions.");
        return false;
    }

    // Bringing it all together. If we want validation layer functionality,
    // the pNext member of vk::InstanceCreateInfo must point to the
    // structure assembled above.
    const vk::InstanceCreateInfo instance_info {
        .pNext = (config.enable_validation ?
                  reinterpret_cast<void *>(&_vvl_features)
                  : nullptr),
        .flags = { },
        .pApplicationInfo = &_app_info,
        .enabledLayerCount =
            static_cast<uint32_t>(_enabled_layers.size()),
        .ppEnabledLayerNames = _enabled_layers.data(),
        .enabledExtensionCount =
            static_cast<uint32_t>(_enabled_extensions.size()),
        .ppEnabledExtensionNames = _enabled_extensions.data()
    };

    auto const result = vk::createInstance(
        &instance_info,
        nullptr,
        &_handle
    );

    // If this didn't work, we can go no further.
    if(result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to create Vulkan instance: '{}'",
                     vk::to_string(result));
        return false;
    }

    // Inform the dynamic dispatcher that we've got an instance.
    VULKAN_HPP_DEFAULT_DISPATCHER.init(_handle);

    BTX_TRACE(
        "Created Vulkan v{}.{}.{} instance: {}",
        VK_API_VERSION_MAJOR(_app_info.apiVersion),
        VK_API_VERSION_MINOR(_app_info.apiVersion),
        VK_API_VERSION_PATCH(_app_info.apiVersion),
        _handle
    );

    if(config.enable_validation) {
        vkDebugger::create();
    }

    return true;
}

// =============================================================================
bool vkInstance::destroy() {
    if(_handle == nullptr) {
        BTX_ERROR("Must create vkInstance before calling destroy().");
        return false;
    }

    vkDebugger::destroy();

    BTX_TRACE(
        "Destroying Vulkan v{}.{}.{} instance: {}",
        VK_API_VERSION_MAJOR(_app_info.apiVersion),
        VK_API_VERSION_MINOR(_app_info.apiVersion),
        VK_API_VERSION_PATCH(_app_info.apiVersion),
        _handle
    );

    _handle.destroy();
    _handle = nullptr;

    return true;
}

// =============================================================================
void vkInstance::_init_dynamic_loader() {
    // The dynamic loader needs something to boostrap itself, so provide it a
    // pointer to find the instance at least
    auto vkGetInstanceProcAddr =
        _loader.getProcAddress<PFN_vkGetInstanceProcAddr>(
            "vkGetInstanceProcAddr"
        );

    // Now we're ready to let it run
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
}

// =============================================================================
void vkInstance::_init_app_info() {
    _app_info.pApplicationName   = _app_name.data();
    _app_info.applicationVersion = _app_version;
    _app_info.pEngineName        = BTX_NAME;
    _app_info.engineVersion      = BTX_VERSION;
    _app_info.apiVersion         = BTX_VK_TARGET_VERSION;
}

// =============================================================================
void vkInstance::_init_validation() {
    // The validation layer helps you know if you've strayed too far from the
    // expected path. It's also extremely opinionated, so each message should
    // be considered individually.
    _enabled_layers.emplace_back("VK_LAYER_KHRONOS_validation");

    // We'll need the debug messenger to receive validation layer messages
    _enabled_extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    // Next we configure what we want the validation layers to report
    _vvl_enabled = {
        vk::ValidationFeatureEnableEXT::eBestPractices,
        vk::ValidationFeatureEnableEXT::eSynchronizationValidation,
        // vk::ValidationFeatureEnableEXT::eDebugPrintf,
        vk::ValidationFeatureEnableEXT::eGpuAssisted,
        vk::ValidationFeatureEnableEXT::eGpuAssistedReserveBindingSlot,
    };

    // Perhaps this'll be useful some time?
    _vvl_disabled = { };

    // Build the final structure
    _vvl_features = {
        .enabledValidationFeatureCount =
            static_cast<uint32_t>(_vvl_enabled.size()),
        .pEnabledValidationFeatures = _vvl_enabled.data(),
        .disabledValidationFeatureCount =
            static_cast<uint32_t>(_vvl_disabled.size()),
        .pDisabledValidationFeatures = _vvl_disabled.data(),
    };
}

// =============================================================================
bool vkInstance::_check_layers() {
    auto const layers = vk::enumerateInstanceLayerProperties();
    BTX_TRACE("Found {} instance layers.", layers.size());

    bool all_layers_supported = true;
    for(auto const * const layer_name : _enabled_layers) {
        BTX_TRACE("Requesting instance layer '{}'", layer_name);

        bool layer_found = false;
        for(auto const &layer : layers) {
            if(std::strcmp(layer_name, layer.layerName) == 0) {
                layer_found = true;
                break;
            }
        }

        if(!layer_found) {
            BTX_WARN("No support for instance extension '{}'", layer_name);
            all_layers_supported = false;
        }
    }

    return all_layers_supported;
}

// =============================================================================
bool vkInstance::_check_extensions() {
    auto const extensions = vk::enumerateInstanceExtensionProperties();
    BTX_TRACE("Found {} instance extensions.", extensions.size());

    bool all_extensions_supported = true;
    for(auto const * const ext_name : _enabled_extensions) {
        BTX_TRACE("Requesting instance extension '{}'", ext_name);

        bool extension_found = false;
        for(auto const &extension : extensions) {
            if(std::strcmp(ext_name, extension.extensionName) == 0) {
                extension_found = true;
                break;
            }
        }

        if(!extension_found) {
            BTX_WARN("No support for instance extension '{}'", ext_name);
            all_extensions_supported = false;
        }
    }

    return all_extensions_supported;
}

} // namespace btx