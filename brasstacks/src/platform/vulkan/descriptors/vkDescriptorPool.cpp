#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorPool.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"

namespace btx {

// =============================================================================
vkDescriptorPool::vkDescriptorPool() :
    _handle { nullptr },
    _device { nullptr }
{ }

// =============================================================================
vkDescriptorPool::~vkDescriptorPool() {
    if(_handle != nullptr) {
        destroy();
    }
}

// =============================================================================
void vkDescriptorPool::create(vk::DescriptorPoolCreateFlags const flags,
                              uint32_t const max_sets,
                              PoolSizes const &sizes)
{
    if(_handle != nullptr) {
        BTX_CRITICAL("Descriptor pool {} already exists", _handle);
        return;
    }

    _device = Renderer::device().native();

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

    _handle = _device.createDescriptorPool(create_info);
    BTX_TRACE("Created descriptor pool {}", _handle);
}

// =============================================================================
void vkDescriptorPool::destroy() {
    BTX_TRACE("Destroying descriptor pool {}", _handle);
    _device.destroyDescriptorPool(_handle);
    _handle = nullptr;
}

} // namespace btx