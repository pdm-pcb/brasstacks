#ifndef BRASSTACKS_PLATFORM_VULKAN_DESCRIPTORS_VKDESCRIPTORSET_HPP
#define BRASSTACKS_PLATFORM_VULKAN_DESCRIPTORS_VKDESCRIPTORSET_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/platform/vulkan/resources/vkBuffer.hpp"
#include "brasstacks/platform/vulkan/resources/vkImage.hpp"

namespace btx {

class vkDevice;
class vkDescriptorPool;
class vkDescriptorSetLayout;

class vkDescriptorSet final {
public:
    vkDescriptorSet(vkDevice const &device, vkDescriptorPool const &pool,
                    vkDescriptorSetLayout const &layout);

    ~vkDescriptorSet() = default;

    vkDescriptorSet & add_buffer(vkBuffer const &buffer,
                                 vk::DescriptorType const type);
    vkDescriptorSet & add_image(vkImage const &image,
                                 vk::DescriptorType const type);

    vkDescriptorSet & allocate();

    void write_set();

    inline auto const & native() const { return _handle; }
    inline auto const & layout() const { return _layout; }

    vkDescriptorSet() = delete;

    vkDescriptorSet(vkDescriptorSet &&) = delete;
    vkDescriptorSet(const vkDescriptorSet &) = delete;

    vkDescriptorSet& operator=(vkDescriptorSet &&) = delete;
    vkDescriptorSet& operator=(const vkDescriptorSet &) = delete;

private:
    vkDevice const &_device;
    vkDescriptorPool const &_pool;
    vkDescriptorSetLayout const &_layout;

    std::vector<vkBuffer> _buffers;
    std::vector<vkImage>  _images;

    std::list<vk::DescriptorBufferInfo> _buffer_info;
    std::list<vk::DescriptorImageInfo>  _image_info;
    std::vector<vk::WriteDescriptorSet> _set_writes;

    vk::DescriptorSet _handle;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_DESCRIPTORS_VKDESCRIPTORSET_HPP