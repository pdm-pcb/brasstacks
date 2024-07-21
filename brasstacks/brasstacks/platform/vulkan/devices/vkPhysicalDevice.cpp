#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"

#include "brasstacks/platform/vulkan/vkInstance.hpp"
#include "brasstacks/platform/vulkan/swapchain/vkSurface.hpp"

namespace btx {

std::vector<vkPhysicalDevice *> vkPhysicalDevice::_available_devices;
vkPhysicalDevice *vkPhysicalDevice::_current_device { nullptr };

// =============================================================================
void vkPhysicalDevice::populate_device_list(
    vkSurface const &surface,
    vk::PhysicalDeviceFeatures2 const &features,
    std::span<char const * const> const extensions)
{
    // Ask the instance for a list of devices
    auto const devices = vkInstance::native().enumeratePhysicalDevices();

    BTX_TRACE("Found {} {}", devices.size(),
              (devices.size() == 1 ? "device" : "devices"));

    // Run through the devices, and only store ones that have what we need
    for(auto const &device : devices) {
        auto *candidate = new vkPhysicalDevice(device);

        if(!candidate->_check_queue_families(surface)) {
            delete candidate;
            continue;
        }

        if(!candidate->_check_features(features)) {
            delete candidate;
            continue;
        }

        if(!candidate->_check_extensions(extensions)) {
            delete candidate;
            continue;
        }

        _available_devices.emplace_back(candidate);
    }

    if(_available_devices.empty()) {
        BTX_CRITICAL("Could not find suitable physical device.");
        return;
    }

    // Sort remaining devices by "performance"
    _sort_devices();

    // And choose the "best"
    _current_device = _available_devices.front();
    BTX_INFO("Selecting {}", _current_device->_name);
}

// =============================================================================
void vkPhysicalDevice::clear_device_list() {
    for(auto *device : _available_devices) {
        delete device;
    }

    _available_devices.clear();
}

// =============================================================================
vkPhysicalDevice & vkPhysicalDevice::current_device() {
    if(_current_device == nullptr) {
        BTX_CRITICAL("No physical device selected.");
    }
    return *_current_device;
}

// =============================================================================
vkPhysicalDevice::vkPhysicalDevice(vk::PhysicalDevice const handle) :
    _handle             { handle },
    _type               { },
    _name               { },
    _vkapi_version      { },
    _vram_bytes         { 0u },
    _driver_version     { },
    _queue_family_index { 0u },
    _samples            { vk::SampleCountFlagBits::e1 },
    _max_aniso          { 0.0f },
    _features           { },
    _features11         { },
    _features12         { },
    _features13         { }
{
    // Retrieve the basic properties of the card
    auto const &device_props = _handle.getProperties();
    _type = device_props.deviceType;
    _name = std::string(device_props.deviceName.data());
    _vkapi_version = fmt::format(
        "{}.{}.{}",
        VK_API_VERSION_MAJOR(device_props.apiVersion),
        VK_API_VERSION_MINOR(device_props.apiVersion),
        VK_API_VERSION_PATCH(device_props.apiVersion)
    );
    _samples   = device_props.limits.framebufferColorSampleCounts;
    _max_aniso = device_props.limits.maxSamplerAnisotropy;

    // And grab some of the sneakier features we're interested in
    _vram_bytes     = _get_vram_bytes(_handle);
    _driver_version = _get_driver_version(_handle);

    BTX_TRACE(
        "\n\n"
        "\tDevice Name:    {}\n"
        "\tDevice Type:    {}\n"
        "\tVRAM:           {} MB\n"
        "\tDriver Version: {}\n"
        "\tVulkan Version: {}",
        _name,
        vk::to_string(_type),
        _vram_bytes / 1000 / 1000,
        _driver_version,
        _vkapi_version
    );
}

// =============================================================================
void vkPhysicalDevice::_sort_devices() {
    // Sort the available devices by VRAM, favoring discrete GPUs
    std::sort(_available_devices.begin(), _available_devices.end(),
        [&](auto const &a, auto const &b)
        {
            if(a->_type == vk::PhysicalDeviceType::eDiscreteGpu &&
               b->_type != vk::PhysicalDeviceType::eDiscreteGpu)
            {
                return true;
            }

            if(a->_type == b->_type) {
                return a->_vram_bytes > b->_vram_bytes;
            }

            return false;
        }
    );
}

// =============================================================================
uint64_t vkPhysicalDevice::_get_vram_bytes(vk::PhysicalDevice const device) {
    auto const &memory_props = device.getMemoryProperties();
    size_t vram_bytes = 0u;
    for(uint32_t index = 0u; index < memory_props.memoryHeapCount; ++index) {
        auto const flags = memory_props.memoryHeaps[index].flags;

        if((flags & vk::MemoryHeapFlagBits::eDeviceLocal) == flags) {
            vram_bytes = memory_props.memoryHeaps[index].size;
            break;
        }
    }
    return vram_bytes;
}

// =============================================================================
std::string
vkPhysicalDevice::_get_driver_version(vk::PhysicalDevice const device) {
    vk::PhysicalDeviceDriverProperties driver_props { };
    vk::PhysicalDeviceProperties2 physical_props2 {
        .pNext = &driver_props
    };
    device.getProperties2(&physical_props2);

    return std::string(driver_props.driverInfo.data());
}

// =============================================================================
void vkPhysicalDevice::_print_family_flags(uint32_t const family,
                                           vk::QueueFlags const flags)
{
    std::string flags_str;
    flags_str.reserve(128);
    flags_str = fmt::format("{}: ", family);

    if(flags & vk::QueueFlagBits::eGraphics) {
        flags_str += "Graphics        ";
    }
    if(flags & vk::QueueFlagBits::eCompute) {
        flags_str += "Compute         ";
    }
    if(flags & vk::QueueFlagBits::eTransfer) {
        flags_str += "Transfer        ";
    }
    if(flags & vk::QueueFlagBits::eSparseBinding) {
        flags_str += "Sparse Binding  ";
    }

#ifdef VK_VERSION_1_1
    if(flags & vk::QueueFlagBits::eProtected) {
        flags_str += "Protected       ";
    }
#endif // VK_VERSION_1_1

#ifdef VK_KHR_video_decode_queue
    if(flags & vk::QueueFlagBits::eVideoDecodeKHR) {
        flags_str += "Video Decode    ";
    }
#endif // VK_KHR_video_decode_queue

#ifdef VK_KHR_video_encode_queue
    if(flags & vk::QueueFlagBits::eVideoEncodeKHR) {
        flags_str += "Video Encode    ";
    }
#endif // VK_KHR_video_encode_queue

#ifdef VK_NV_optical_flow
    if(flags & vk::QueueFlagBits::eOpticalFlowNV) {
        flags_str += "Optical Flow    ";
    }
#endif // VK_NV_optical_flow

    BTX_TRACE("  {}", flags_str);
}

// =============================================================================
bool vkPhysicalDevice::_check_queue_families(vkSurface const &surface) {
    auto const &families = _handle.getQueueFamilyProperties();
    // BTX_TRACE("Found {} queue families for {}", families.size(), _name);

    for(uint32_t i = 0u; i < families.size(); ++i) {
        _print_family_flags(i, families[i].queueFlags);
    }

    bool found_unified_family = false;
    for(uint32_t i = 0u; i < families.size(); ++i) {
        // The first check is if this queue family supports graphics commands
        if(families[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            auto const present_support =
                _handle.getSurfaceSupportKHR(i, surface.native());

            // And the second is if this device can present on the surface
            // we've been given
            if(present_support == VK_TRUE) {
                found_unified_family = true;
                _queue_family_index = i;

                BTX_TRACE("{} queue family index {} supports graphics and "
                          "present.",
                          _name, _queue_family_index);

                break;
            }
        }
    }

    if(!found_unified_family) {
        BTX_WARN("{} doesn't support a unified graphics and present queue.",
                 _name);
    }

    return found_unified_family;
}

// =============================================================================
bool
vkPhysicalDevice::_check_features(vk::PhysicalDeviceFeatures2 const &features) {
    // Copy over the requested features
    _features = features;
    _features11 = *(static_cast<vk::PhysicalDeviceVulkan11Features *>(_features.pNext));
    _features12 = *(static_cast<vk::PhysicalDeviceVulkan12Features *>(_features11.pNext));
    _features13 = *(static_cast<vk::PhysicalDeviceVulkan13Features *>(_features12.pNext));

    // Fix up the local features structure chain
    _features.pNext   = &_features11;
    _features11.pNext = &_features12;
    _features12.pNext = &_features13;
    _features13.pNext = nullptr;

    // Build the structure chain we'll use to query the device
    auto supported13 = vk::PhysicalDeviceVulkan13Features {
        .pNext = nullptr,
    };

    auto supported12 = vk::PhysicalDeviceVulkan12Features {
        .pNext = &supported13,
    };

    auto supported11 = vk::PhysicalDeviceVulkan11Features {
        .pNext = &supported12,
    };

    auto supported = vk::PhysicalDeviceFeatures2 {
        .pNext = &supported11,
    };

    // Ask the device what we're dealing with
    _handle.getFeatures2(&supported);

    // Run through and check what we care about
    bool all_features_supported = true;

    // VK1.0 features ----------------------------------------------------------
    if(_features.features.fillModeNonSolid
       && supported.features.fillModeNonSolid)
    {
        BTX_TRACE("{} supports fillModeNonSolid.", _name);
    }
    else if(_features.features.fillModeNonSolid) {
        BTX_WARN("{} does not support fillModeNonSolid.", _name);
        all_features_supported = false;
    }

    if(_features.features.samplerAnisotropy
       && supported.features.samplerAnisotropy)
    {
        BTX_TRACE("{} supports samplerAnisotropy.", _name);
    }
    else if(_features.features.samplerAnisotropy) {
        BTX_WARN("{} does not samplerAnisotropy.", _name);
        all_features_supported = false;
    }

    // VK1.2 features ----------------------------------------------------------
    if(_features12.uniformAndStorageBuffer8BitAccess
       && supported12.uniformAndStorageBuffer8BitAccess)
    {
        BTX_TRACE("{} supports unfiformAndStorageBuffer8BitAccess.", _name);
    }
    else if(_features12.uniformAndStorageBuffer8BitAccess) {
        BTX_TRACE("{} does not support unfiformAndStorageBuffer8BitAccess.",
                  _name);
        all_features_supported = false;
    }

    if(_features12.bufferDeviceAddress && supported12.bufferDeviceAddress) {
        BTX_TRACE("{} supports bufferDeviceAddress.", _name);
    }
    else if(_features12.bufferDeviceAddress) {
        BTX_TRACE("{} does not support bufferDeviceAddress.", _name);
        all_features_supported = false;
    }

    // VK1.3 features ----------------------------------------------------------
    if(_features13.synchronization2 && supported13.synchronization2) {
        BTX_TRACE("{} supports synchronization2.", _name);
    }
    else if(_features13.synchronization2) {
        BTX_TRACE("{} does not support synchronization2.", _name);
        all_features_supported = false;
    }

    if(_features13.dynamicRendering && supported13.dynamicRendering) {
        BTX_TRACE("{} supports dynamicRendering.", _name);
    }
    else if(_features13.dynamicRendering) {
        BTX_TRACE("{} does not support dynamicRendering.", _name);
        all_features_supported = false;
    }

    return all_features_supported;
}

// =============================================================================
bool
vkPhysicalDevice::_check_extensions(std::span<char const * const> extensions) {
    // Get the list of supported extensions
    auto const supported_extensions =
        _handle.enumerateDeviceExtensionProperties();

    BTX_TRACE("Found {} extensions for {}", supported_extensions.size(),
                                            _name);

    // Run through the required list and the supported list to make sure the
    // latter contains all of the former
    bool all_extensions_supported = true;
    for(char const * const required : extensions) {
        bool extension_found = false;

        for(auto const &supported : supported_extensions) {
            if(::strcmp(required, supported.extensionName) == 0) {
                _extensions.push_back(supported);
                extension_found = true;
                BTX_TRACE("{} supports '{}'", _name, required);
                break;
            }
        }

        if(extension_found == false) {
            BTX_WARN("{} does not support '{}'", _name, required);
            all_extensions_supported = false;
        }
    }

    return all_extensions_supported;
}

} // namespace btx