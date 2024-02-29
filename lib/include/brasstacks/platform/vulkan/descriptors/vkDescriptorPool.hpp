#ifndef BRASSTACKS_PLATFORM_VULKAN_DESCRIPTORS_VKDESCRIPTORPOOL_HPP
#define BRASSTACKS_PLATFORM_VULKAN_DESCRIPTORS_VKDESCRIPTORPOOL_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDevice;

class vkDescriptorPool final {
public:
    using PoolSizes = std::set<vk::DescriptorPoolSize>;
    vkDescriptorPool(vkDevice const &device,
                     vk::DescriptorPoolCreateFlags const flags,
                     uint32_t const max_sets, PoolSizes const &sizes);
    ~vkDescriptorPool();

    inline auto const& native() const { return _handle; }

    vkDescriptorPool() = delete;

    vkDescriptorPool(vkDescriptorPool &&) = delete;
    vkDescriptorPool(const vkDescriptorPool &) = delete;

    vkDescriptorPool& operator=(vkDescriptorPool &&) = delete;
    vkDescriptorPool& operator=(const vkDescriptorPool &) = delete;

private:
    vkDevice const &_device;
    vk::DescriptorPool _handle;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_DESCRIPTORS_VKDESCRIPTORPOOL_HPP