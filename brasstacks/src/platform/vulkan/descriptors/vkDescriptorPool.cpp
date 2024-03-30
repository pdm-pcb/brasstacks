#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorPool.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"

namespace btx {

// =============================================================================
vkDescriptorPool::vkDescriptorPool(vk::DescriptorPoolCreateFlags const flags,
                                   uint32_t const max_sets,
                                   PoolSizes const &sizes)
{
    std::vector<vk::DescriptorPoolSize> const pool_sizes {
        sizes.begin(),
        sizes.end()
    };

    vk::DescriptorPoolCreateInfo const create_info {
        .pNext         = nullptr,
        .flags         = flags,
        .maxSets       = max_sets,
        .poolSizeCount = static_cast<uint32_t>(pool_sizes.size()),
        .pPoolSizes    = pool_sizes.data(),
    };

    _handle = Renderer::device().native().createDescriptorPool(create_info);
    BTX_TRACE("Created descriptor pool {}", _handle);
}

// =============================================================================
vkDescriptorPool::~vkDescriptorPool() {
    BTX_TRACE("Destroying descriptor pool {}", _handle);
    Renderer::device().native().destroyDescriptorPool(_handle);
}

} // namespace btx