#ifndef BRASSTACKS_PLATFORM_VULKAN_DESCRIPTORS_VKDESCRIPTORSET_HPP
#define BRASSTACKS_PLATFORM_VULKAN_DESCRIPTORS_VKDESCRIPTORSET_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/platform/vulkan/resources/buffers/vkBuffer.hpp"
#include "brasstacks/platform/vulkan/resources/images/vkImage.hpp"

namespace btx {

class vkDevice;
class vkDescriptorPool;
class vkDescriptorSetLayout;

class vkDescriptorSet {
public:
    vkDescriptorSet(vkDevice const &device, vkDescriptorPool const &pool,
                    vkDescriptorSetLayout const &layout);

    ~vkDescriptorSet() = default;

    vkDescriptorSet & add_buffer(vkBuffer const &buffer);
    vkDescriptorSet & add_image(vkImage const &image);

    vkDescriptorSet & allocate();

    void write_set();

    inline auto const& native() const { return _handle; }

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

    vk::DescriptorSet _handle;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_DESCRIPTORS_VKDESCRIPTORSET_HPP