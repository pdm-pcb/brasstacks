#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"

#include "brasstacks/platform/vulkan/vkInstance.hpp"
#include "brasstacks/platform/vulkan/rendering/vkSurface.hpp"
#include "brasstacks/system/TargetWindow.hpp"

namespace btx {

// =============================================================================
vkPhysicalDevice::vkPhysicalDevice(vkInstance    const &instance,
                                   vkSurface     const &surface,
                                   FeatureList   const &required_features,
                                   ExtensionList const &required_extensions) :
    _available_devices  { },
    _chosen_device      { }
{
    // Populate a list of devices we can choose from and sort by "performance"
    _enumerate_and_sort(instance);

    for(auto &device : _available_devices) {
        // Check that the card supports receiving graphics and presentation
        // commands on the same queue family. Most consumer GPUs will support
        // this on index zero.
        if(_check_queue_families(device, surface) == false) {
            BTX_WARN("Skipping {}", device.name);
            continue;
        }
        BTX_TRACE("Queue index {} supports graphics and present commands.",
                  device.graphics_queue_index);

        // Check that the card supports all the features passed in
        if(_check_features(device, required_features) == false) {
            BTX_WARN("Skipping {}", device.name);
            continue;
        }
        BTX_TRACE("Found {} required features.", required_features.size());

        // Check that the card supports all the extensions passed in
        if(_check_extensions(device, required_extensions) == false) {
            BTX_WARN("Skipping {}", device.name);
            continue;
        }
        BTX_TRACE("Found {} required extensions.", required_extensions.size());

        // If we've made it this far, this is our card!
        _chosen_device = device;
        break;
    }

    if(!_chosen_device.handle) {
        BTX_CRITICAL("Could not find suitable phsyical device.");
        return;
    }

    BTX_INFO("Selected {}", _chosen_device.name);
}

// =============================================================================
void vkPhysicalDevice::_enumerate_and_sort(vkInstance const &instance) {
    // Populate the list of physical devices
    auto const result = instance.native().enumeratePhysicalDevices();
    if(result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to enumerate physical devices.");
        return;
    }

    auto const &devices = result.value;
    BTX_TRACE("Found {} {}", devices.size(),
              (devices.size() == 1 ? "device" : "devices"));

    for(auto const &device : devices) {
        // Add the device and details to our list of cards to choose from
        _store_device(device);
    }

    // Sort the available devices by VRAM, favoring discrete GPUs
    std::sort(
        _available_devices.begin(), _available_devices.end(),
        [&](const DeviceProps &dev_a, const DeviceProps &dev_b)
        {
            if(dev_a.type == vk::PhysicalDeviceType::eDiscreteGpu &&
               dev_b.type != vk::PhysicalDeviceType::eDiscreteGpu)
            {
                return true;
            }

            if((dev_a.type == vk::PhysicalDeviceType::eDiscreteGpu &&
                dev_b.type == vk::PhysicalDeviceType::eDiscreteGpu) ||
            (dev_a.type == vk::PhysicalDeviceType::eIntegratedGpu &&
                dev_b.type == vk::PhysicalDeviceType::eIntegratedGpu))
            {
                return dev_a.vram_bytes > dev_b.vram_bytes;
            }

            return false;
        }
    );

    // Print the sorted list of cards
    for(auto const &properties : _available_devices) {
        BTX_TRACE(
            "\n"
            "\tDevice Name:    {}\n"
            "\tDevice Type:    {}\n"
            "\tVRAM:           {} MB\n"
            "\tDriver Version: {}\n"
            "\tVulkan Version: {}\n",
            properties.name,
            vk::to_string(properties.type),
            properties.vram_bytes / 1000 / 1000,
            properties.driver_version,
            properties.vkapi_version
        );
    }
}

// =============================================================================
bool vkPhysicalDevice::_check_queue_families(DeviceProps &device,
                                             vkSurface const &surface)
{
    bool found_unified_family = false;

    auto const &families = device.handle.getQueueFamilyProperties();
    BTX_TRACE("Found {} queue families for {}", families.size(), device.name);

    for(uint32_t i = 0u; i < families.size(); ++i) {
        _print_family_flags(i, families[i].queueFlags);
    }

    for(uint32_t i = 0u; i < families.size(); ++i) {
        // The first check is if this queue family supports graphics commands
        if(families[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            auto const present_support =
                device.handle.getSurfaceSupportKHR(i, surface.native());

            // And the second is if this device can present on the surface
            // we've been given
            if(present_support.result == vk::Result::eSuccess) {
                found_unified_family = true;
                device.graphics_queue_index = i;
                break;
            }
        }
    }

    return found_unified_family;
}

// =============================================================================
bool vkPhysicalDevice::_check_features(DeviceProps &device,
                                       FeatureList const &required_features)
{
    bool all_features_supported = true;

    // Grab the supported features struct
    auto const &supported_features = device.handle.getFeatures();

    // And run through our required features. The supported features struct is
    // just a bunch of boolean values, so we have to add an explicit block to
    // check each one we're interested in.
    for(auto const &feature : required_features) {
        if(feature == Features::SAMPLER_ANISOTROPY) {
            if(supported_features.samplerAnisotropy != VK_TRUE) {
                BTX_WARN("No support for sampler anisotropy.");
                all_features_supported = false;
                break;
            }

            device.enabled_features.samplerAnisotropy = VK_TRUE;
        }
        else if(feature == Features::FILL_MODE_NONSOLID) {
            if(supported_features.fillModeNonSolid != VK_TRUE) {
                BTX_WARN("No support for non-solid fill modes.");
                all_features_supported = false;
                break;
            }

            device.enabled_features.fillModeNonSolid = VK_TRUE;
        }
        else {
            BTX_CRITICAL("Unknown physical device feature requested.");
            all_features_supported = false;
            break;
        }
    }

    return all_features_supported;
}

// =============================================================================
bool
vkPhysicalDevice::_check_extensions(DeviceProps &device,
                                    ExtensionList const &required_extensions)
{
    bool all_extensions_supported = true;

    // Get the list of supported extensions
    auto const result = device.handle.enumerateDeviceExtensionProperties();
    if(result.result != vk::Result::eSuccess) {
        BTX_ERROR("Failed to enumerate physical device extensions.");
        return false;
    }

    auto const &supported_extensions = result.value;
    BTX_TRACE("Found {} extensions for {}", supported_extensions.size(),
                                            device.name);

    // Run through the required list and the supported list to make sure the
    // latter contains all of the former
    for(auto const required : required_extensions) {
        bool extension_found = false;

        for(auto const &supported : supported_extensions) {
            if(::strcmp(required.data(), supported.extensionName) == 0) {
                device.enabled_extensions.push_back(required.data());
                extension_found = true;
                break;
            }
        }

        if(extension_found == false) {
            BTX_WARN("{} does not support extension '{}'",
                     device.name, required);
            all_extensions_supported = false;
        }
    }

    return all_extensions_supported;
}

// =============================================================================
void vkPhysicalDevice::_store_device(vk::PhysicalDevice const &device) {
    // Retrieve the basic properties of the card
    auto const &device_props = device.getProperties();

    // We'll pull the VRAM amount from here
    auto const &memory_props = device.getMemoryProperties();

    // And the driver version from here
    vk::PhysicalDeviceDriverProperties driver_props { };
    vk::PhysicalDeviceProperties2KHR physical_props2 {
        .pNext = &driver_props
    };
    device.getProperties2(&physical_props2);

    // Time to start filling things in
    _available_devices.emplace_back(DeviceProps { });
    auto &store = _available_devices.back();

    store.handle = device;
    store.memory = memory_props;
    store.type = device_props.deviceType;
    store.name = std::string(device_props.deviceName.data());

    size_t vram_bytes = 0;
    for(uint32_t index = 0u; index < memory_props.memoryHeapCount; ++index) {
        auto const flags = memory_props.memoryHeaps[index].flags;

        if((flags & vk::MemoryHeapFlagBits::eDeviceLocal) == flags) {
            vram_bytes = memory_props.memoryHeaps[index].size;
            break;
        }
    }
    store.vram_bytes = vram_bytes;

    store.driver_version = std::string(driver_props.driverInfo.data());

    store.vkapi_version = std::format(
        "{}.{}.{}",
        VK_API_VERSION_MAJOR(device_props.apiVersion),
        VK_API_VERSION_MINOR(device_props.apiVersion),
        VK_API_VERSION_PATCH(device_props.apiVersion)
    );
}

// =============================================================================
void vkPhysicalDevice::_print_family_flags(uint32_t const family,
                                           vk::QueueFlags const flags)
{
    std::string flags_str;
    flags_str.reserve(128);
    flags_str = std::format("{}: ", family);

    if(flags & vk::QueueFlagBits::eGraphics) {
        flags_str += "Graphics       ";
    }
    if(flags & vk::QueueFlagBits::eCompute) {
        flags_str += "Compute        ";
    }
    if(flags & vk::QueueFlagBits::eTransfer) {
        flags_str += "Transfer       ";
    }
    if(flags & vk::QueueFlagBits::eSparseBinding) {
        flags_str += "Sparse Binding ";
    }
    if(flags & vk::QueueFlagBits::eProtected) {
        flags_str += "Protected      ";
    }
    if(flags & vk::QueueFlagBits::eVideoDecodeKHR) {
        flags_str += "Video Decode   ";
    }

    BTX_TRACE("    {}", flags_str);
}

} // namespace btx