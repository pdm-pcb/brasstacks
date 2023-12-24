#ifndef BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKRENDERPASS_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKRENDERPASS_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDevice;

class vkRenderPass {
public:
    auto const & native() const { return _handle; }

    vkRenderPass(vkDevice const &device, vk::Format const format,
                 vk::SampleCountFlagBits const msaa_samples);
    ~vkRenderPass();

    vkRenderPass() = delete;

    vkRenderPass(vkRenderPass &&) = delete;
    vkRenderPass(vkRenderPass const &) = delete;

    vkRenderPass & operator=(vkRenderPass &&) = delete;
    vkRenderPass & operator=(vkRenderPass const &) = delete;

private:
    vkDevice const &_device;

    vk::RenderPass _handle;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKRENDERPASS_HPP