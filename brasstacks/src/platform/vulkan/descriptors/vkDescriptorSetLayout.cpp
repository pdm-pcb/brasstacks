#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorSetLayout.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"

namespace btx {

// =============================================================================
vkDescriptorSetLayout::vkDescriptorSetLayout() :
    _handle   { nullptr },
    _bindings { },
    _device   { nullptr }
{ }

// =============================================================================
vkDescriptorSetLayout::~vkDescriptorSetLayout() {
    if(_handle != nullptr) {
        destroy();
    }
}

// =============================================================================
vkDescriptorSetLayout &
vkDescriptorSetLayout::add_binding(vk::DescriptorType const type,
                                   vk::ShaderStageFlags const stages,
                                   uint32_t const descriptor_count)
{
    if(_handle != nullptr) {
        BTX_CRITICAL("Descriptor set layout {} already exists", _handle);
    }

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
    if(_handle != nullptr) {
        BTX_CRITICAL("Descriptor set layout {} already exists", _handle);
    }

    _device = Renderer::device().native();

    const vk::DescriptorSetLayoutCreateInfo descriptor_info {
        .bindingCount = static_cast<uint32_t>(_bindings.size()),
        .pBindings    = _bindings.data(),
    };

    auto const result = _device.createDescriptorSetLayout(
        &descriptor_info,
        nullptr,
        &_handle
    );

    if(result != vk::Result::eSuccess) {
        BTX_CRITICAL("Unable to create descriptor set layout: '{}'",
                     vk::to_string(result));
    }

    BTX_TRACE("Created descriptor set layout {}", _handle);
}

// =============================================================================
void vkDescriptorSetLayout::destroy() {
    BTX_TRACE("Destroying descriptor set layout {}", _handle);
    _device.destroyDescriptorSetLayout(_handle);
    _handle = nullptr;
}

} // namespace btx