#ifndef BRASSTACKS_PLATFORM_VULKAN_PIPELINE_VKSHADERPROGRAM_HPP
#define BRASSTACKS_PLATFORM_VULKAN_PIPELINE_VKSHADERPROGRAM_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkShaderStage;

class vkShaderProgram final {
public:
    vkShaderProgram();
    ~vkShaderProgram();

    vkShaderProgram & add_stage(std::string_view const filepath);

    void create();
    void destroy();

    vkShaderProgram(vkShaderProgram &&rhs);
    vkShaderProgram(vkShaderProgram const &) = delete;

    vkShaderProgram& operator=(vkShaderProgram &&) = delete;
    vkShaderProgram& operator=(vkShaderProgram const &) = delete;

private:
    std::vector<vkShaderStage> _stages;
    std::vector<vk::PushConstantRange> _push_constants;
    std::vector<vk::DescriptorSetLayoutBinding> _desc_sets;

    void _merge_push_constants();
    void _merge_descriptor_sets();
    bool _update_binding_stage(vk::DescriptorSetLayoutBinding const &binding);
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_PIPELINE_VKSHADERPROGRAM_HPP