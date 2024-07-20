#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"

#include "brasstacks/platform/vulkan/vkInstance.hpp"
#include "brasstacks/platform/vulkan/swapchain/vkSurface.hpp"

namespace btx {

std::vector<vkPhysicalDevice *> vkPhysicalDevice::_available_devices;
vkPhysicalDevice const *vkPhysicalDevice::_current_device { nullptr };

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
vkPhysicalDevice::vkPhysicalDevice(vk::PhysicalDevice const handle) :
    _handle             { handle },
    _type               { },
    _name               { },
    _vkapi_version      { },
    _vram_bytes         { 0u },
    _driver_version     { },
    _queue_family_index { 0u },
    _samples            { vk::SampleCountFlagBits::e1 },
    _max_aniso          { 0.0f }
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

    _samples = device_props.limits.framebufferColorSampleCounts;
    _max_aniso = device_props.limits.maxSamplerAnisotropy;

    // Pulling the VRAM count from here
    _vram_bytes = _get_vram_bytes(_handle);

    // And the driver version from here
    vk::PhysicalDeviceDriverProperties driver_props { };
    vk::PhysicalDeviceProperties2 physical_props2 {
        .pNext = &driver_props
    };
    _handle.getProperties2(&physical_props2);

    _driver_version = std::string(driver_props.driverInfo.data());

    // Set up the features structure chain
    _enabled_features13.pNext = nullptr;
    _enabled_features12.pNext = &_enabled_features13;
    _enabled_features11.pNext = &_enabled_features12;
    _enabled_features.pNext   = &_enabled_features11;

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
vkPhysicalDevice::_check_features(vk::PhysicalDeviceFeatures2 const &features)
{
    // First, pull the required features structs out for easier comparison
    auto const &features11 =
        *(static_cast<vk::PhysicalDeviceVulkan11Features *>(features.pNext));

    auto const &features12 =
        *(static_cast<vk::PhysicalDeviceVulkan12Features *>(features11.pNext));

    auto const &features13 =
        *(static_cast<vk::PhysicalDeviceVulkan13Features *>(features12.pNext));

    // Next, build the struct chain to ask the device what we're working with
    auto supported13 = vk::PhysicalDeviceVulkan13Features {
        .pNext = nullptr,
    };

    auto supported12 = vk::PhysicalDeviceVulkan12Features {
        .pNext = &supported13,
    };

    auto supported11 = vk::PhysicalDeviceVulkan11Features {
        .pNext = &supported12,
    };

    auto supported2 = vk::PhysicalDeviceFeatures2 {
        .pNext = &supported11,
    };

    _handle.getFeatures2(&supported2);

    // Now check the features we care about
    bool all_features_supported = true;

    if(features.features.fillModeNonSolid
       && supported2.features.fillModeNonSolid)
    {
        BTX_TRACE("{} supports fillModeNonSolid.", _name);
        _enabled_features.features.samplerAnisotropy = VK_TRUE;
    }
    else if(features.features.fillModeNonSolid) {
        BTX_WARN("{} does not support fillModeNonSolid.", _name);
        all_features_supported = false;
    }

    if(features.features.samplerAnisotropy
       && supported2.features.samplerAnisotropy)
    {
        BTX_TRACE("{} supports samplerAnisotropy.", _name);
        _enabled_features.features.fillModeNonSolid = VK_TRUE;
    }
    else if(features.features.samplerAnisotropy) {
        BTX_WARN("{} does not samplerAnisotropy.", _name);
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
                _enabled_extensions.push_back(supported);
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