#ifndef BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKCOLORDEPTHPASS_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKCOLORDEPTHPASS_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDevice;

class vkColorDepthPass {
public:
    auto const & native() const { return _handle; }

    vkColorDepthPass(vkDevice const &device, vk::Format const format,
                     vk::SampleCountFlagBits const msaa_samples);
    ~vkColorDepthPass();

    vkColorDepthPass() = delete;

    vkColorDepthPass(vkColorDepthPass &&) = delete;
    vkColorDepthPass(vkColorDepthPass const &) = delete;

    vkColorDepthPass & operator=(vkColorDepthPass &&) = delete;
    vkColorDepthPass & operator=(vkColorDepthPass const &) = delete;

private:
    vkDevice const &_device;

    vk::RenderPass _handle;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKCOLORDEPTHPASS_HPP