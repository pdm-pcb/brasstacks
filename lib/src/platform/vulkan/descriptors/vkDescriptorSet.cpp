#include "brasstacks/brasstacks.hpp"

#include "brasstacks/platform/vulkan/descriptors/vkDescriptorSet.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorPool.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorSetLayout.hpp"
#include "brasstacks/platform/vulkan/resources/vkImage.hpp"
#include "brasstacks/platform/vulkan/resources/vkImageView.hpp"
#include "brasstacks/platform/vulkan/resources/vkSampler.hpp"

namespace btx {

// =============================================================================
vkDescriptorSet::vkDescriptorSet(vkDevice const &device,
                                 vkDescriptorPool const &pool,
                                 vkDescriptorSetLayout const &layout) :
    _device { device },
    _pool   { pool },
    _layout { layout }
{
    const vk::DescriptorSetAllocateInfo alloc_info {
        .descriptorPool     = _pool.native(),
        .descriptorSetCount = 1u,
        .pSetLayouts        = &_layout.native()
    };

    auto const result = _device.native().allocateDescriptorSets(alloc_info);
    _handle = result.front();
    BTX_TRACE("Allocated descriptor set {}", _handle);
}

// =============================================================================
vkDescriptorSet & vkDescriptorSet::add_buffer(vkBuffer const &buffer,
                                              vk::DescriptorType const type)
{
    if(!_handle) {
        BTX_CRITICAL("Must allocate descriptor set before adding buffers.");
    }

    auto const *buffer_info = &_buffer_info.emplace_back(
        vk::DescriptorBufferInfo {
            .buffer = buffer.native(),
            .offset = 0u,
            .range  = VK_WHOLE_SIZE
        });

    _set_writes.emplace_back(vk::WriteDescriptorSet {
        .dstSet           = _handle,
        .dstBinding       = static_cast<uint32_t>(_set_writes.size()),
        .dstArrayElement  = 0u,
        .descriptorCount  = 1u,
        .descriptorType   = type,
        .pImageInfo       = nullptr,
        .pBufferInfo      = buffer_info,
        .pTexelBufferView = nullptr
    });

    return *this;
}

// =============================================================================
vkDescriptorSet & vkDescriptorSet::add_image(vkImage const &image,
                                             vkImageView const &view,
                                             vkSampler const &sampler,
                                             vk::DescriptorType const type)
{
    if(!_handle) {
        BTX_CRITICAL("Must allocate descriptor set before adding images.");
    }

    auto const *image_info =  &_image_info.emplace_back(
        vk::DescriptorImageInfo {
            .sampler = sampler.native(),
            .imageView = view.native(),
            .imageLayout = image.layout(),
        });

    _set_writes.emplace_back(vk::WriteDescriptorSet {
        .dstSet           = _handle,
        .dstBinding       = static_cast<uint32_t>(_set_writes.size()),
        .dstArrayElement  = 0u,
        .descriptorCount  = 1u,
        .descriptorType   = type,
        .pImageInfo       = image_info,
        .pBufferInfo      = nullptr,
        .pTexelBufferView = nullptr
    });

    return *this;
}

// =============================================================================
void vkDescriptorSet::write_set() {
    if(_set_writes.empty()) {
        BTX_ERROR("Trying to update descriptor set with no set writes.");
        return;
    }

    _device.native().updateDescriptorSets(_set_writes, nullptr);

    _buffer_info.clear();
    _image_info.clear();
    _set_writes.clear();
}

} // namespace btx