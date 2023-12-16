#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"

#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"

namespace btx {

// =============================================================================
void vkDevice::wait_idle() const {
    auto const result = _handle.waitIdle();
    if(result != vk::Result::eSuccess) {
        BTX_WARN("Failed to wait for logical device idle.");
    }
}

// =============================================================================
vkDevice::vkDevice(vkPhysicalDevice const &adapter, Layers const &layers) :
    _cmd_queue { *this }
{
    // The first step in device creation is to tell the queue what index it
    // will be using.
    _cmd_queue.fill_create_info(adapter.queue_index());

    // Then ask the queue for the populated structure
    vk::DeviceQueueCreateInfo const queue_info[] {
        _cmd_queue.create_info()
    };

    // The logical device wants to know what the physical device has to offer
    auto const &extensions = adapter.extensions();
    auto const *features   = &(adapter.features());

    // That should be everything we need
    const vk::DeviceCreateInfo device_info {
        .queueCreateInfoCount    = static_cast<uint32_t>(std::size(queue_info)),
        .pQueueCreateInfos       = queue_info,
        .enabledLayerCount       = static_cast<uint32_t>(layers.size()),
        .ppEnabledLayerNames     = layers.data(),
        .enabledExtensionCount   = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
        .pEnabledFeatures        = features,
    };

    // Attempt creation
    auto const result = adapter.native().createDevice(
        &device_info,
        nullptr,
        &_handle
    );

    // Make sure it worked
    if(result != vk::Result::eSuccess) {
        BTX_CRITICAL("Unable to create logical device: '{}'",
                     vk::to_string(result));
        return;
    }

    BTX_TRACE("Created logical device {:#x}",
                reinterpret_cast<uint64_t>(::VkDevice(_handle)));

    // Once the logical device is established, the queue can likewise come
    // online
    _cmd_queue.request_queue();

    // This is the final step in providing the dynamic loader with information
    VULKAN_HPP_DEFAULT_DISPATCHER.init(_handle);
}

vkDevice::~vkDevice() {
    BTX_TRACE("Destroying logical device {:#x}",
              reinterpret_cast<uint64_t>(::VkDevice(_handle)));
    _handle.destroy();
}

} // namespace btx