#include "brasstacks/platform/vulkan/devices/vkLogicalDevice.hpp"

#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"

namespace btx {

// =============================================================================
void vkLogicalDevice::submit(vk::SubmitInfo const &submit_info) const {
    auto const result = _cmd_queue.native().submit(submit_info);
    if(result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to submit to command queue.");
    }
}

// =============================================================================
void vkLogicalDevice::wait_idle() const {
    auto const result = _handle.waitIdle();
    if(result != vk::Result::eSuccess) {
        BTX_WARN("Failed to wait for logical device idle.");
    }
}

// =============================================================================
vkLogicalDevice::vkLogicalDevice(vkPhysicalDevice const &adapter,
                                 Layers const &layers) :
    _cmd_queue { *this },
    _transient_pool { *this },
    _adapter { adapter }
{
    // The first step in device creation is to tell the queue what index it
    // will be using.
    _cmd_queue.fill_create_info(_adapter.queue_index());

    // Then ask the queue for the populated structure
    vk::DeviceQueueCreateInfo queue_info[] {
        _cmd_queue.create_info()
    };

    // The logical device wants to know what the physical device has to offer
    auto const &extensions = _adapter.extensions();
    auto const * features   = &(_adapter.features());

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
    auto const result = _adapter.native().createDevice(
        &device_info,
        nullptr,
        &_handle
    );

    // Make sure it worked
    if(result != vk::Result::eSuccess) {
        BTX_CRITICAL(
            "Unable to create logical device: '{}'",
            vk::to_string(result)
        );
    }
    else {
        BTX_TRACE(
            "Created logical device {:#x}",
            reinterpret_cast<uint64_t>(VkDevice(native()))
        );
    }

    // Once the logical device is established, the queue can likewise come
    // online
    _cmd_queue.request_queue();

    // This is the final step in providing the dynamic loader with information
    VULKAN_HPP_DEFAULT_DISPATCHER.init(_handle);

    // Create the dedicated command pool for one-use command buffers
    _transient_pool.create(vk::CommandPoolCreateFlagBits::eTransient);
}

vkLogicalDevice::~vkLogicalDevice() {
    _transient_pool.destroy();

    BTX_TRACE(
        "Destroying logical device {:#x}",
        reinterpret_cast<uint64_t>(VkDevice(native()))
    );
    _handle.destroy();
}

} // namespace btx