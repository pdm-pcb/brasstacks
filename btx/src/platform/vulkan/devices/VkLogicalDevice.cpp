#include "brasstacks/platform/vulkan/devices/VkLogicalDevice.hpp"

#include "brasstacks/platform/vulkan/devices/VkPhysicalDevice.hpp"

namespace btx {

VkCmdQueue VkLogicalDevice::_cmd_queue      { };
VkCmdPool  VkLogicalDevice::_transient_pool { };
vk::Device VkLogicalDevice::_logical_device { };

// =============================================================================
void VkLogicalDevice::create(std::vector<char const *> const &layer_names) {
    // The first step in device creation is to tell the queue what index it
    // will be using.
    _cmd_queue.fill_create_info(VkPhysicalDevice::queue_index());

    // Then ask the queue for the populated structure
    vk::DeviceQueueCreateInfo queue_info[] {
        _cmd_queue.create_info()
    };

    // The logical device wants to know what the physical device has to offer
    auto const &extensions = VkPhysicalDevice::extensions();
    auto const * features   = &(VkPhysicalDevice::features());

    // That should be everything we need
    const vk::DeviceCreateInfo device_info {
        .queueCreateInfoCount    = static_cast<uint32_t>(std::size(queue_info)),
        .pQueueCreateInfos       = queue_info,
        .enabledLayerCount       = static_cast<uint32_t>(layer_names.size()),
        .ppEnabledLayerNames     = layer_names.data(),
        .enabledExtensionCount   = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
        .pEnabledFeatures        = features,
    };

    // Attempt creation
    auto const result = VkPhysicalDevice::native().createDevice(
        &device_info,
        nullptr,
        &_logical_device
    );

    // Make sure it worked
    if(result != vk::Result::eSuccess) {
        BTX_CRITICAL(
            "Unable to create logical device: '{}'",
            to_string(result)
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
    VULKAN_HPP_DEFAULT_DISPATCHER.init(_logical_device);

    // Create the dedicated command pool for one-use command buffers
    _transient_pool.create(vk::CommandPoolCreateFlagBits::eTransient);
}

// =============================================================================
void VkLogicalDevice::destroy() {
    BTX_TRACE(
        "Destroying logical device {:#x}",
        reinterpret_cast<uint64_t>(VkDevice(native()))
    );

    _transient_pool.destroy();
    _logical_device.destroy();
}

// =============================================================================
void VkLogicalDevice::submit_to_cmd_queue(vk::SubmitInfo const &submit_info) {
    auto const result = _cmd_queue.native().submit(submit_info);
    if(result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to submit to command queue.");
    }
}

// =============================================================================
void VkLogicalDevice::wait_idle() {
    auto const result = _logical_device.waitIdle();
    if(result != vk::Result::eSuccess) {
        BTX_WARN("Failed to wait for logical device idle.");
    }
}

} // namespace btx