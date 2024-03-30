#ifndef BRASSTACKS_PLATFORM_VULKAN_PASSES_VKRENDERPASS_HPP
#define BRASSTACKS_PLATFORM_VULKAN_PASSES_VKRENDERPASS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/core/Renderer.hpp"

namespace btx {

class vkRenderPassBase {
public:
    auto const & native() const { return _handle; }

    virtual ~vkRenderPassBase() = default;

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