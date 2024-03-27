#ifndef BRASSTACKS_PLATFORM_VULKAN_PASSES_VKRENDERPASS_HPP
#define BRASSTACKS_PLATFORM_VULKAN_PASSES_VKRENDERPASS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/core/Renderer.hpp"

namespace btx {

class vkRenderPassBase {
public:
    auto const & native() const { return _handle; }

    virtual ~vkRenderPassBase();

    vkRenderPassBase() = delete;

    vkRenderPassBase(vkRenderPassBase &&) = delete;
    vkRenderPassBase(vkRenderPassBase const &) = delete;

    vkRenderPassBase & operator=(vkRenderPassBase &&) = delete;
    vkRenderPassBase & operator=(vkRenderPassBase const &) = delete;

protected:
    explicit vkRenderPassBase(Renderer const &renderer);

    void _create(vk::RenderPassCreateInfo const &create_info);

    auto const & device() const { return _renderer.device(); }
    auto const & swapchain() const { return _renderer.swapchain(); }

private:
    Renderer const &_renderer;

    vk::RenderPass _handle;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_PASSES_VKRENDERPASS_HPP