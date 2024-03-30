#ifndef BRASSTACKS_PLATFORM_VULKAN_DESCRIPTORS_VKDESCRIPTORPOOL_HPP
#define BRASSTACKS_PLATFORM_VULKAN_DESCRIPTORS_VKDESCRIPTORPOOL_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDescriptorPool final {
public:
    vkDescriptorPool();
    ~vkDescriptorPool();

    using PoolSizes = std::set<vk::DescriptorPoolSize>;
    void create(vk::DescriptorPoolCreateFlags const flags,
                uint32_t const max_sets, PoolSizes const &sizes);
    void destroy();

    inline auto const& native() const { return _handle; }

    vkDescriptorPool(vkDescriptorPool &&) = delete;
    vkDescriptorPool(const vkDescriptorPool &) = delete;

    vkDescriptorPool& operator=(vkDescriptorPool &&) = delete;
    vkDescriptorPool& operator=(const vkDescriptorPool &) = delete;

private:
    vk::DescriptorPool _handle;
    vk::Device _device;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_DESCRIPTORS_VKDESCRIPTORPOOL_HPP