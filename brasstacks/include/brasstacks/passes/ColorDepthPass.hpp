#ifndef BRASSTACKS_PASSES_COLORDEPTHPASS_HPP
#define BRASSTACKS_PASSES_COLORDEPTHPASS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkPipeline.hpp"

namespace btx {

class Renderer;

class vkColorDepthPass;
class vkFramebuffer;
class vkCmdBuffer;

class vkDescriptorSetLayout;
class vkDescriptorSet;

class ColorDepthPass final {
    using PushConstants = std::span<vkPipeline::PushConstant const>;

public:
    ColorDepthPass();
    ~ColorDepthPass();

    void create_pipeline();
    void create_swapchain_resources();
    void destroy_swapchain_resources();

    void begin();
    void end();

    void bind_descriptor_set(vkDescriptorSet const &set) const;

    void send_push_constants(PushConstants const push_constants);

    auto & pipeline() { return *_pipeline; }

    ColorDepthPass(ColorDepthPass &&) = delete;
    ColorDepthPass(ColorDepthPass const &) = delete;

    ColorDepthPass & operator=(ColorDepthPass &&) = delete;
    ColorDepthPass & operator=(ColorDepthPass const &) = delete;

private:
    vkColorDepthPass            *_render_pass;
    vkPipeline                  *_pipeline;
    std::vector<vkFramebuffer *> _framebuffers;

    vkCmdBuffer const *_cmd_buffer;

    void _create_framebuffers();
    void _destroy_framebuffers();
};

} // namespace btx

#endif // BRASSTACKS_PASSES_COLORDEPTHPASS_HPP