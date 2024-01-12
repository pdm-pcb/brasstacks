#ifndef BRASSTACKS_RENDERING_PASSES_COLORDEPTHPASS_HPP
#define BRASSTACKS_RENDERING_PASSES_COLORDEPTHPASS_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDevice;
class vkSwapchain;
class vkColorDepthPass;
class vkPipeline;
class vkFramebuffer;

class vkCmdBuffer;

class ColorDepthPass {
public:
    ColorDepthPass(vkDevice const &device, vkSwapchain const &swapchain);
    ~ColorDepthPass();

    void begin(vkCmdBuffer const &cmd_buffer, uint32_t const image_index);
    void end();

    ColorDepthPass() = delete;

    ColorDepthPass(ColorDepthPass &&) = delete;
    ColorDepthPass(ColorDepthPass const &) = delete;

    ColorDepthPass & operator=(ColorDepthPass &&) = delete;
    ColorDepthPass & operator=(ColorDepthPass const &) = delete;

private:
    vkDevice const &_device;
    vkSwapchain const &_swapchain;

    vkColorDepthPass                 *_render_pass;
    vkPipeline                  *_pipeline;
    std::vector<vkFramebuffer *> _framebuffers;

    vkCmdBuffer const *_cmd_buffer;

    void _create_pipeline();
    void _create_framebuffers();
};

} // namespace btx

#endif // BRASSTACKS_RENDERING_PASSES_COLORDEPTHPASS_HPP