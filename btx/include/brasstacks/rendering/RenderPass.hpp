#ifndef BRASSTACKS_RENDERING_RENDERPASS_HPP
#define BRASSTACKS_RENDERING_RENDERPASS_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDevice;
class vkSwapchain;
class vkDescriptorSetLayout;
class vkRenderPass;
class vkPipeline;
class vkFramebuffer;

class RenderPass {
public:
    RenderPass(vkDevice const &device, vkSwapchain const &swapchain,
               vkDescriptorSetLayout const &layout);
    ~RenderPass();

    RenderPass() = delete;

private:
    vkRenderPass                *_render_pass;
    vkPipeline                  *_pipeline;
    std::vector<vkFramebuffer *> _framebuffers;
};

} // namespace btx

#endif // BRASSTACKS_RENDERING_RENDERPASS_HPP