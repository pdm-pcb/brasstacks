#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/vkInstance.hpp"

#include "brasstacks/platform/vulkan/vkDebugger.hpp"

// This (and more) does away with the explicit loading of each
// function/extension. See: https://github.com/KhronosGroup/Vulkan-Hpp
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace btx {

vk::Instance vkInstance::_handle { nullptr };

vk::DynamicLoader         vkInstance::_loader { };
uint32_t                  vkInstance::_target_api_version { };
vk::ApplicationInfo       vkInstance::_app_info { };
std::vector<char const *> vkInstance::_enabled_layers { };
std::vector<char const *> vkInstance::_enabled_extensions { };

std::vector<vk::ValidationFeatureEnableEXT> vkInstance::_vvl_enabled { };
vk::ValidationFeaturesEXT vkInstance::_vvl_features { };

// =============================================================================
void vkInstance::create(uint32_t const api_version) {
    if(_handle) {
        BTX_CRITICAL("Vulkan instance {} already exists", _handle);
        return;
    }

    _target_api_version = api_version;

    _init_dynamic_loader(); // The first step for using the dynamic loader
    _init_app_info();       // Provide hints about this app to the driver
    _init_layers();         // Init the validation layer, if we're in debug
    _init_extensions();     // Extensions are often implementation defined

    // Run through the extensions the driver offers and make sure we've got
    // what we need
    if(!_check_layers()) {
        BTX_CRITICAL("Could not get support for all requested instance "
                     "layers.");
        return;
    }

    // Run through the extensions the driver offers and make sure we've got
    // what we need
    if(!_check_extensions()) {
        BTX_CRITICAL("Could not get support for all requested instance "
                     "extensions.");
        return;
    }

    // Bringing it all together. If we want validation layer functionality,
    // the pNext member of vk::InstanceCreateInfo must point to the
    // structure assembled above.
    const vk::InstanceCreateInfo instance_info {
#ifdef BTX_DEBUG
        .pNext = reinterpret_cast<void *>(&_vvl_features),
#else
        .pNext = nullptr,
#endif // BTX_DEBUG
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
        BTX_CRITICAL(
            "Failed to create Vulkan instance: '{}'",
            vk::to_string(result)
        );
        return;
    }

    // Inform the dynamic dispatcher that we've got an instance.
    VULKAN_HPP_DEFAULT_DISPATCHER.init(_handle);

    BTX_INFO(
        "Created Vulkan v{}.{}.{} instance: {}",
        VK_API_VERSION_MAJOR(_app_info.apiVersion),
        VK_API_VERSION_MINOR(_app_info.apiVersion),
        VK_API_VERSION_PATCH(_app_info.apiVersion),
        _handle
    );

#ifdef BTX_DEBUG
    vkDebugger::create();
#endif // BTX_DEBUG
}

// =============================================================================
void vkInstance::destroy() {
#ifdef BTX_DEBUG
    vkDebugger::destroy();
#endif // BTX_DEBUG

    BTX_TRACE("Destroying Vulkan instance {}", _handle);
    _handle.destroy();
    _handle = nullptr;
}

// =============================================================================
void vkInstance::_init_dynamic_loader() {
    using instance_proc = PFN_vkGetInstanceProcAddr;

    // The dynamic loader needs something to boostrap itself, so provide it a
    // pointer to find the instance at least
    auto vkGetInstanceProcAddr = _loader.getProcAddress<instance_proc>(
        "vkGetInstanceProcAddr"
    );

    // Now we're ready to let it run
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
}

// =============================================================================
void vkInstance::_init_app_info() {
    _app_info.pApplicationName   = nullptr;
    _app_info.applicationVersion = 0u;
    _app_info.pEngineName        = BTX_NAME;
    _app_info.engineVersion      = BTX_VERSION;
    _app_info.apiVersion         = _target_api_version;
}

// =============================================================================
void vkInstance::_init_layers() {
#ifdef BTX_DEBUG
    // The validation layer helps you know if you've strayed too far from the
    // expected path. It's also extremely opinionated, so each message should
    // be considered individually.
    _enabled_layers = { "VK_LAYER_KHRONOS_validation" };
#endif // BTX_DEBUG
}

// =============================================================================
void vkInstance::_init_extensions() {
    _enabled_extensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);

    // Surfaces describe the spaces to which you can draw in Vulkan. They're
    // also platform dependant.
#ifdef BTX_LINUX
    _enabled_extensions.emplace_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#elif BTX_WINDOWS
    _enabled_extensions.emplace_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif // BTX platform

    // And for dynamic rendering:
    _enabled_extensions.emplace_back(
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
    );

#ifdef BTX_DEBUG
    // The first steps toward giving the driver a path to keep us abreast of
    // myriad details.
    _enabled_extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    _enabled_extensions.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

    // As with the last lines, these features support our debugging efforts
    _vvl_enabled = {
        vk::ValidationFeatureEnableEXT::eBestPractices,
        vk::ValidationFeatureEnableEXT::eSynchronizationValidation,
        // vk::ValidationFeatureEnableEXT::eDebugPrintf,
        vk::ValidationFeatureEnableEXT::eGpuAssisted,
        vk::ValidationFeatureEnableEXT::eGpuAssistedReserveBindingSlot,
    };

    _vvl_features = {
        .enabledValidationFeatureCount =
            static_cast<uint32_t>(_vvl_enabled.size()),
        .pEnabledValidationFeatures = _vvl_enabled.data(),
        .disabledValidationFeatureCount = 0u,
        .pDisabledValidationFeatures = nullptr
    };
#endif // BTX_DEBUG
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