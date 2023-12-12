#include "brasstacks/platform/vulkan/vkInstance.hpp"

#include "brasstacks/platform/vulkan/vkDebugger.hpp"

// This (and more) does away with the explicit loading of each
// function/extension. See: https://github.com/KhronosGroup/Vulkan-Hpp
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace btx {

// =============================================================================
vkInstance::vkInstance() :
    _target_api_version { VK_API_VERSION_1_3 }
{
    static std::once_flag initialized;
    std::call_once(initialized, [&] {
        _init_dynamic_loader(); // The first step for using the dynamic loader
        _init_app_info();       // Provide hints about this app to the driver
        _init_layers();         // Init the validation layer, if we're in debug
        _init_extensions();     // Extensions are often implementation defined

        auto const ext_result = vk::enumerateInstanceExtensionProperties();
        if(ext_result.result != vk::Result::eSuccess) {
            BTX_CRITICAL("Failed to enumerate instance extensions.");
            return;
        }

        auto const &extensions = ext_result.value;
        BTX_TRACE("Found {} instance extensions.", extensions.size());

        // Run through the extensions the driver offers and make sure we've got
        // what we need
        if(!_check_extensions(extensions)) {
            return;
        }

        // Bringing it all together. If we want validation layer functionality,
        // the pNext member of vk::InstanceCreateInfo must point to the
        // structure assembled above.
        const vk::InstanceCreateInfo instance_info {
            .pNext = reinterpret_cast<void *>(&_validation_extensions),
            .flags = { },
            .pApplicationInfo = &_app_info,
            .enabledLayerCount =
                static_cast<std::uint32_t>(_enabled_layers.size()),
            .ppEnabledLayerNames = _enabled_layers.data(),
            .enabledExtensionCount =
                static_cast<std::uint32_t>(_enabled_extensions.size()),
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
            "Created Vulkan v{}.{}.{} instance",
            VK_API_VERSION_MAJOR(_app_info.apiVersion),
            VK_API_VERSION_MINOR(_app_info.apiVersion),
            VK_API_VERSION_PATCH(_app_info.apiVersion)
        );

#ifdef BTX_DEBUG
        vkDebugger::init(_handle);
#endif // BTX_DEBUG
    });
}

// =============================================================================
vkInstance::~vkInstance() {
#ifdef BTX_DEBUG
    vkDebugger::shutdown(_handle);
#endif // BTX_DEBUG

    _handle.destroy();
}

// =============================================================================
void vkInstance::_init_dynamic_loader() {
    using inst_proc = PFN_vkGetInstanceProcAddr; // A little brevity
    auto vkGetInstanceProcAddr = _loader.getProcAddress<inst_proc>(
        "vkGetInstanceProcAddr"
    );
    // Bootstrap the auto-loader
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
}

// =============================================================================
// Several static constexpr values loaded in from the central header
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
#if defined(BTX_LINUX)
    _enabled_extensions.emplace_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#elif defined(BTX_WINDOWS)
    _enabled_extensions.emplace_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif // BTX platform

#ifdef BTX_DEBUG
    // The first steps toward giving the drive a path to keep us abreast of
    // myriad details.
    _enabled_extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    _enabled_extensions.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

    // As with the last line, these features support our debugging efforts
    _validation_features = {
        vk::ValidationFeatureEnableEXT::eGpuAssisted,
        vk::ValidationFeatureEnableEXT::eGpuAssistedReserveBindingSlot,
        vk::ValidationFeatureEnableEXT::eBestPractices,

        // DebugPrintf and GpuAssisted are mutually exclusive. DebugPrintf is
        // very handy when used in conjucntion with RenderDoc, but I'm opting
        // for more self-contained guidance for now.
        // vk::ValidationFeatureEnableEXT::eDebugPrintf,

        // Last time I checked, there were false positives/bugs around
        // synchronization validation on Linux
        #if defined(BTX_WINDOWS)
            vk::ValidationFeatureEnableEXT::eSynchronizationValidation
        #endif
    };
#endif // BTX_DEBUG

    _validation_extensions = {
        .enabledValidationFeatureCount =
            static_cast<std::uint32_t>(_validation_features.size()),
        .pEnabledValidationFeatures = _validation_features.data(),
        .disabledValidationFeatureCount = 0u,
        .pDisabledValidationFeatures = nullptr
    };

    for(auto const& extension : _enabled_extensions) {
        BTX_TRACE("Requesting instance extension '{}'", extension);
    }
}

// =============================================================================
bool vkInstance::_check_extensions(Extensions const &supported_extensions) {
    for(auto const * const ext_name : _enabled_extensions) {
        bool extension_found = false;

        for(auto const &extension : supported_extensions) {
            if(std::strcmp(ext_name, extension.extensionName) == 0) {
                extension_found = true;
                break;
            }
        }
        if(!extension_found) {
            BTX_WARN("No support for instance extension '{}'", ext_name);
            return false;
        }
    }

    return true;
}

} // namespace btx