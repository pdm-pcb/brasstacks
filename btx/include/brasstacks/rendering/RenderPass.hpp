#ifndef BRASSTACKS_RENDERING_RENDERPASS_HPP
#define BRASSTACKS_RENDERING_RENDERPASS_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDevice;
class vkSwapchain;
class vkDescriptorSetLayout;
class vkDescriptorSet;
class vkRenderPass;
class vkPipeline;
class vkFramebuffer;
class vkCmdBuffer;

class RenderPass {
public:
    RenderPass(vkDevice const &device, vkSwapchain const &swapchain,
               vkDescriptorSetLayout const &layout);

    ~RenderPass();

    void begin(vkCmdBuffer const &cmd_buffer, uint32_t const image_index) const;
    void bind(vkCmdBuffer const &cmd_buffer,
              vkDescriptorSet const &descriptor_set) const;

    RenderPass() = delete;

private:
    vkRenderPass                *_render_pass;
    vkPipeline                  *_pipeline;
    std::vector<vkFramebuffer *> _framebuffers;
    vk::Rect2D                   _render_area;
};

} // namespace btx

#endif // BRASSTACKS_RENDERING_RENDERPASS_HPP