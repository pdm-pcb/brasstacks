#include "brasstacks/core.hpp"
#include "brasstacks/platform/vulkan/vulkan_formatters.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorPool.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"

namespace btx {

// =============================================================================
vkDescriptorPool::vkDescriptorPool(vkDevice const &device,
                                   uint32_t const max_sets,
                                   PoolSizes const &sizes) :
    _device { device }
{
    std::vector<vk::DescriptorPoolSize> const pool_sizes {
        sizes.begin(),
        sizes.end()
    };

    vk::DescriptorPoolCreateInfo const create_info {
        .pNext         = nullptr,
        .flags         = { },
        .maxSets       = max_sets,
        .poolSizeCount = static_cast<uint32_t>(pool_sizes.size()),
        .pPoolSizes    = pool_sizes.data(),
    };

    auto const result = _device.native().createDescriptorPool(create_info);
    if(result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to create descriptor set pool: '{}'",
                     vk::to_string(result.result));
        return;
    }

    _handle = result.value;
    BTX_TRACE("Created descriptor pool {}", _handle);
}

// =============================================================================
vkDescriptorPool::~vkDescriptorPool() {
    BTX_TRACE("Destroying descriptor pool {}", _handle);
    _device.native().destroyDescriptorPool(_handle);
}

} // namespace btx