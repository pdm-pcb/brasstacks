#ifndef BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKRENDERPASS_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKRENDERPASS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/platform/vulkan/rendering/vkSwapchain.hpp"

namespace btx {

class vkDevice;

class vkRenderPass {
public:
    auto const & native() const { return _handle; }

    virtual ~vkRenderPass();

    vkRenderPass() = delete;

    vkRenderPass(vkRenderPass &&) = delete;
    vkRenderPass(vkRenderPass const &) = delete;

    vkRenderPass & operator=(vkRenderPass &&) = delete;
    vkRenderPass & operator=(vkRenderPass const &) = delete;

protected:
    vkRenderPass(vkDevice const &device, vkSwapchain const &swapchain);

    void _create(vk::RenderPassCreateInfo const &create_info);

    auto const & device() const { return _device; }
    auto const & swapchain() const { return _swapchain; }

private:
    vkDevice const &_device;
    vkSwapchain const &_swapchain;

    vk::RenderPass _handle;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKRENDERPASS_HPP