#ifndef BRASSTACKS_PLATFORM_VULKAN_DESCRIPTORS_VKDESCRIPTORSET_HPP
#define BRASSTACKS_PLATFORM_VULKAN_DESCRIPTORS_VKDESCRIPTORSET_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDescriptorPool;
class vkDescriptorSetLayout;

class vkBuffer;
class vmaBuffer;

class vkImage;
class vkImageView;
class vkSampler;

class vkDescriptorSet final {
public:
    vkDescriptorSet();

    // Descriptor sets don't get freed because the pools we're using haven't
    // been created with VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT.
    ~vkDescriptorSet() = default;

    void allocate(vkDescriptorPool const &pool,
                  vkDescriptorSetLayout const &layout);

    vkDescriptorSet & add_buffer(vkBuffer const &buffer,
                                 vk::DescriptorType const type);

    vkDescriptorSet & add_buffer(vmaBuffer const &buffer,
                                 vk::DescriptorType const type);

    vkDescriptorSet & add_image(vkImage const &image, vkImageView const &view,
                                vkSampler const &sampler,
                                vk::DescriptorType const type);

    void write_set();

    inline auto const & native() const { return _handle; }
    inline auto const & layout() const { return _layout; }

    vkDescriptorSet(vkDescriptorSet &&) = delete;
    vkDescriptorSet(const vkDescriptorSet &) = delete;

    vkDescriptorSet& operator=(vkDescriptorSet &&) = delete;
    vkDescriptorSet& operator=(const vkDescriptorSet &) = delete;

private:
    vk::DescriptorSet _handle;

    vk::Device _device;
    vk::DescriptorPool _pool;
    vk::DescriptorSetLayout _layout;

    std::list<vk::DescriptorBufferInfo> _buffer_info;
    std::list<vk::DescriptorImageInfo>  _image_info;
    std::vector<vk::WriteDescriptorSet> _set_writes;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_DESCRIPTORS_VKDESCRIPTORSET_HPP