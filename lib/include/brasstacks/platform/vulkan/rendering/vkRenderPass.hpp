#ifndef BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKRENDERPASS_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKRENDERPASS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/core/Renderer.hpp"

namespace btx {

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
    vkRenderPass(Renderer const &renderer);

    void _create(vk::RenderPassCreateInfo const &create_info);

    auto const & device() const { return _renderer.device(); }
    auto const & swapchain() const { return _renderer.swapchain(); }

private:
    Renderer const &_renderer;

    vk::RenderPass _handle;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKRENDERPASS_HPP