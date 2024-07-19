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

    vkShaderProgram(vkShaderProgram &&rhs) = delete;
    vkShaderProgram(const vkShaderProgram &) = delete;

    vkShaderProgram& operator=(vkShaderProgram &&) = delete;
    vkShaderProgram& operator=(const vkShaderProgram &) = delete;

private:
    vk::Device _device;
    std::vector<vkShaderStage> _stages;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_PIPELINE_VKSHADERPROGRAM_HPP