#include "brasstacks/platform/vulkan/descriptors/vkDescriptorSet.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorPool.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorSetLayout.hpp"

namespace btx {

// =============================================================================
vkDescriptorSet::vkDescriptorSet(vkDevice const &device,
                                 vkDescriptorPool const &pool,
                                 vkDescriptorSetLayout const &layout) :
    _device { device },
    _pool   { pool },
    _layout { layout }
{ }

// =============================================================================
vkDescriptorSet & vkDescriptorSet::add_buffer(vkBuffer const &buffer) {
    // _buffers.push_back(buffer);
    return *this;
}

// =============================================================================
vkDescriptorSet & vkDescriptorSet::add_image(vkImage const &image) {
    // _images.push_back(image);
    return *this;
}

// =============================================================================
vkDescriptorSet & vkDescriptorSet::allocate() {
    const vk::DescriptorSetAllocateInfo alloc_info {
        .descriptorPool = _pool.native(),
        .descriptorSetCount = 1u,
        .pSetLayouts = &_layout.native()
    };

    auto const result = _device.native().allocateDescriptorSets(
        &alloc_info,
        &_handle
    );

    if(result != vk::Result::eSuccess) {
        BTX_CRITICAL("Could not allocate descriptor sets: '{}'",
                     vk::to_string(result));
    }

    return *this;
}

// =============================================================================
void vkDescriptorSet::write_set() {
    std::vector<vk::DescriptorBufferInfo> buffer_info;
    buffer_info.reserve(_buffers.size());

    for(auto const& buffer : _buffers) {
        buffer_info.push_back({
            .buffer = buffer.native(),
            .offset = 0u,
            .range = VK_WHOLE_SIZE
        });
    }

    _buffers.clear();

    std::vector<vk::DescriptorImageInfo> image_info;
    image_info.reserve(_images.size());

    // for(auto const& image : _images) {
    //     image_info.emplace_back(vk::DescriptorImageInfo {
    //         .sampler     = image.sampler(),
    //         .imageView   = image.view(),
    //         .imageLayout = image.layout()
    //     });
    // }

    _images.clear();

    auto const buffer_count = static_cast<uint32_t>(buffer_info.size());
    auto const image_count = static_cast<uint32_t>(image_info.size());

    std::vector<vk::WriteDescriptorSet> set_writes;
    set_writes.reserve(2);

    if(buffer_count > 0) {
        set_writes.emplace_back(vk::WriteDescriptorSet {
            .dstSet = _handle,
            .dstBinding = 0u,
            .dstArrayElement = 0u,
            .descriptorCount = buffer_count,
            .descriptorType = vk::DescriptorType::eUniformBuffer,
            .pImageInfo = nullptr,
            .pBufferInfo = buffer_info.data(),
            .pTexelBufferView = nullptr
        });
    }

    if(image_count > 0) {
        set_writes.emplace_back(vk::WriteDescriptorSet {
            .dstSet = _handle,
            .dstBinding = 0u,
            .dstArrayElement = 0u,
            .descriptorCount = image_count,
            .descriptorType = vk::DescriptorType::eCombinedImageSampler,
            .pImageInfo = image_info.data(),
            .pBufferInfo = nullptr,
            .pTexelBufferView = nullptr
        });
    }

    _device.native().updateDescriptorSets(set_writes, nullptr);
}

} // namespace btx