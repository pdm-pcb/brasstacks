#include "brasstacks/platform/vulkan/descriptors/vkDescriptorSetLayout.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"

namespace btx {

// =============================================================================
vkDescriptorSetLayout::vkDescriptorSetLayout(vkDevice const &device) :
    _device { device }
{ }

// =============================================================================
vkDescriptorSetLayout::~vkDescriptorSetLayout() {
    BTX_TRACE("Destroying descriptor set layout {:#x}",
              reinterpret_cast<uint64_t>(VkDescriptorSetLayout(_handle)));

    _device.native().destroyDescriptorSetLayout(_handle);
}

// =============================================================================
vkDescriptorSetLayout &
vkDescriptorSetLayout::add_binding(vk::DescriptorType const type,
                                   vk::ShaderStageFlags const stages,
                                   uint32_t const descriptor_count)
{
    _bindings.emplace_back(vk::DescriptorSetLayoutBinding {
        .binding         = static_cast<uint32_t>(_bindings.size()),
        .descriptorType  = type,
        .descriptorCount = descriptor_count,
        .stageFlags      = stages,
    });

    BTX_TRACE("Adding Descriptor type: {} binding: {} ",
              vk::to_string(type), _bindings.back().binding);

    return *this;
}

// =============================================================================
void vkDescriptorSetLayout::create() {
    const vk::DescriptorSetLayoutCreateInfo descriptor_info {
        .bindingCount = static_cast<uint32_t>(_bindings.size()),
        .pBindings    = _bindings.data(),
    };

    auto const result = _device.native().createDescriptorSetLayout(
        &descriptor_info,
        nullptr,
        &_handle
    );

    if(result != vk::Result::eSuccess) {
        BTX_CRITICAL("Unable to create descriptor set layout: '{}'",
                     vk::to_string(result));
    }

    BTX_TRACE("Created descriptor set layout {:#x}",
              reinterpret_cast<uint64_t>(VkDescriptorSetLayout(_handle)));
}

} // namespace btx