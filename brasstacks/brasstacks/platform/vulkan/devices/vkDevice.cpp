#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"

#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkQueue.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBufferPool.hpp"

namespace btx {

// =============================================================================
vkDevice::vkDevice() :
    _handle         { nullptr },
    _graphics_queue { new vkQueue },
    _transient_pool { new vkCmdBufferPool }
{ }

// =============================================================================
vkDevice::~vkDevice() {
    if(_handle) {
        destroy();
    }

    delete _graphics_queue;
    delete _transient_pool;
}

// =============================================================================
void vkDevice::create() {
    if(_handle) {
        BTX_ERROR("vkDevice already created.");
        return;
    }

    if(_graphics_queue->native()) {
        BTX_ERROR("Graphics command queue already created.");
        return;
    }

    if(_transient_pool->native()) {
        BTX_ERROR("Transient command buffer pool already created.");
        return;
    }

    // We'll be asking the current physical device for its logical
    // representation
    auto &physical_device = vkPhysicalDevice::current_device();

    // We only need one device queue, so only need to specify one priority
    float const queue_priorities[] = { 1.0f };

    // Populate the device queue create struct
    vk::DeviceQueueCreateInfo const queue_info[] {{
        .pNext = nullptr,
        .flags = { },
        .queueFamilyIndex = physical_device.queue_family_index(),
        .queueCount = static_cast<uint32_t>(std::size(queue_priorities)),
        .pQueuePriorities = queue_priorities,
    }};

    // The logical device concerns itself with the features and extensions
    // enabled on the physical device
    auto const &features = physical_device.features();
    auto const &extensions = physical_device.extensions();

    std::vector<char const *> extension_names;
    extension_names.reserve(extensions.size());

    for(auto const &extension : extensions) {
        extension_names.emplace_back(extension.extensionName);
    }

    // Now populate the device's create struct
    vk::DeviceCreateInfo const device_create_info {
        .pNext                   = &features,
        .flags                   = { },
        .queueCreateInfoCount    = static_cast<uint32_t>(std::size(queue_info)),
        .pQueueCreateInfos       = queue_info,
        .enabledExtensionCount   = static_cast<uint32_t>(extension_names.size()),
        .ppEnabledExtensionNames = extension_names.data(),
        .pEnabledFeatures        = nullptr,
    };

    // And try to create it
    auto const result = physical_device.native().createDevice(
        &device_create_info, // Create info
        nullptr,             // Allocator
        &_handle             // Destination handle
    );

    // Check that we've got good results to work with
    if(result != vk::Result::eSuccess || !_handle) {
        BTX_CRITICAL("Unable to create logical device: '{}'",
                     vk::to_string(result));
        return;
    }

    BTX_TRACE("Created logical device {}", _handle);

    // Set up the queue abstraction
    _graphics_queue->create(physical_device.queue_family_index());

    // This is the final step in providing the dynamic loader with information
    VULKAN_HPP_DEFAULT_DISPATCHER.init(_handle);

    _transient_pool->create(_graphics_queue->family_index(),
                            vk::CommandPoolCreateFlagBits::eTransient);
}

// =============================================================================
void vkDevice::destroy() {
    _transient_pool->destroy();
    _graphics_queue->destroy();

    BTX_TRACE("Destroying logical device {}", _handle);
    _handle.destroy();
    _handle = nullptr;
}

// =============================================================================
void vkDevice::wait_idle() const {
    BTX_TRACE("Waiting for device idle...");
    _handle.waitIdle();
}

} // namespace btx