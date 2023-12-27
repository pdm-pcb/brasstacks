#include "brasstacks/core.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"

#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkQueue.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdPool.hpp"

namespace btx {

// =============================================================================
vkDevice::vkDevice(vkPhysicalDevice const &physical_device,
                   Layers const &layers) :
    _handle         { nullptr },
    _graphics_queue { nullptr },
    _transient_pool { nullptr }
{
    // We only need one device queue, so only need to specify one priority
    float const queue_priorities[] = { 1.0f };

    // Populate the device queue create struct
    vk::DeviceQueueCreateInfo const queue_info[] {{
        .pNext = nullptr,
        .flags = { },
        .queueFamilyIndex = physical_device.graphics_queue_index(),
        .queueCount = static_cast<uint32_t>(std::size(queue_priorities)),
        .pQueuePriorities = queue_priorities,
    }};

    // The logical device wants to know what the physical device has enabled
    auto const &extensions = physical_device.extensions();
    auto const *features   = &(physical_device.features());

    // Now populate the device's create struct
    const vk::DeviceCreateInfo device_info {
        .queueCreateInfoCount    = static_cast<uint32_t>(std::size(queue_info)),
        .pQueueCreateInfos       = queue_info,
        .enabledLayerCount       = static_cast<uint32_t>(layers.size()),
        .ppEnabledLayerNames     = layers.data(),
        .enabledExtensionCount   = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
        .pEnabledFeatures        = features,
    };

    // And try to create it
    auto const result = physical_device.native().createDevice(
        &device_info,   // Create info
        nullptr,        // Allocator
        &_handle        // Destination handle
    );

    // Check that we've got good results to work with
    if(result != vk::Result::eSuccess || !_handle) {
        BTX_CRITICAL("Unable to create logical device: '{}'",
                     vk::to_string(result));
        return;
    }

    BTX_TRACE("Created logical device {}", _handle);

    // Retrieve the queue abstraction
    _graphics_queue =
        new vkQueue(*this, physical_device.graphics_queue_index());

    // This is the final step in providing the dynamic loader with information
    VULKAN_HPP_DEFAULT_DISPATCHER.init(_handle);

    _transient_pool = new vkCmdPool(
        *this,
        physical_device.graphics_queue_index(),
        vk::CommandPoolCreateFlagBits::eTransient
    );
}

// =============================================================================
vkDevice::~vkDevice() {
    delete _transient_pool;
    delete _graphics_queue;

    BTX_TRACE("Destroying logical device {}", _handle);
    _handle.destroy();
}

// =============================================================================
void vkDevice::wait_idle() const {
    auto const result = _handle.waitIdle();
    if(result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to wait for logical device idle.");
    }
}

} // namespace btx