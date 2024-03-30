#ifndef BRASSTACKS_PLATFORM_VULKAN_DESCRIPTORS_VKDESCRIPTORSETLAYOUT_HPP
#define BRASSTACKS_PLATFORM_VULKAN_DESCRIPTORS_VKDESCRIPTORSETLAYOUT_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDescriptorSetLayout final {
public:
    vkDescriptorSetLayout() = default;
    ~vkDescriptorSetLayout();

    vkDescriptorSetLayout & add_binding(vk::DescriptorType const type,
                                        vk::ShaderStageFlags const stages,
                                        uint32_t const descriptor_count = 1u);

    void create();

    inline auto const& native()   const { return _handle;   }
    inline auto const& bindings() const { return _bindings; }

    vkDescriptorSetLayout(vkDescriptorSetLayout &&) = delete;
    vkDescriptorSetLayout(const vkDescriptorSetLayout &) = delete;

    vkDescriptorSetLayout& operator=(vkDescriptorSetLayout &&) = delete;
    vkDescriptorSetLayout& operator=(const vkDescriptorSetLayout &) = delete;

private:
    using LayoutBindings = std::vector<vk::DescriptorSetLayoutBinding>;
    LayoutBindings _bindings;
    vk::DescriptorSetLayout _handle;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_DESCRIPTORS_VKDESCRIPTORSETLAYOUT_HPP