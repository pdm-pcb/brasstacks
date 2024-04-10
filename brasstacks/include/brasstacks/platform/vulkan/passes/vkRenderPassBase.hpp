#ifndef BRASSTACKS_PLATFORM_VULKAN_PASSES_VKRENDERPASS_HPP
#define BRASSTACKS_PLATFORM_VULKAN_PASSES_VKRENDERPASS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/core/Renderer.hpp"

namespace btx {

class vkFramebuffer;

class vkRenderPassBase {
public:
    virtual ~vkRenderPassBase() = default;

    virtual void create() = 0;
    virtual void destroy() = 0;

    virtual void create_swapchain_resources() = 0;
    virtual void destroy_swapchain_resources() = 0;

    virtual void begin(vkFramebuffer const &framebuffer) = 0;
    virtual void end() = 0;

    auto const & native() const { return _handle; }

    vkRenderPassBase(vkRenderPassBase &&) = delete;
    vkRenderPassBase(vkRenderPassBase const &) = delete;

    vkRenderPassBase & operator=(vkRenderPassBase &&) = delete;
    vkRenderPassBase & operator=(vkRenderPassBase const &) = delete;

protected:
    vkRenderPassBase();

    void _create(vk::RenderPassCreateInfo const &create_info);
    void _destroy();

private:
    vk::RenderPass _handle;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_PASSES_VKRENDERPASS_HPP