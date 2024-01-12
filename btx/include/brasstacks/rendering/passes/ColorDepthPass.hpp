#ifndef BRASSTACKS_RENDERING_PASSES_COLORDEPTHPASS_HPP
#define BRASSTACKS_RENDERING_PASSES_COLORDEPTHPASS_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class Renderer;

class vkColorDepthPass;
class vkPipeline;
class vkFramebuffer;
class vkCmdBuffer;

class vkDescriptorSetLayout;
class vkDescriptorSet;

class ColorDepthPass {
public:
    explicit ColorDepthPass(Renderer const &renderer);
    ~ColorDepthPass();

    void create();

    void destroy_swapchain_resources();
    void recreate_swapchain_resources();

    void begin();
    void end();

    void bind_descriptor_set(vkDescriptorSet const &set) const;

    struct PushConstant {
        vk::ShaderStageFlags const stage_flags = vk::ShaderStageFlagBits::eAll;
        size_t const size_bytes = 0;
        void const *data = nullptr;
    };

    using PushConstants = std::vector<PushConstant>;

    void send_push_constants(PushConstants const &push_constants);

    auto & pipeline() { return *_pipeline; }

    ColorDepthPass() = delete;

    ColorDepthPass(ColorDepthPass &&) = delete;
    ColorDepthPass(ColorDepthPass const &) = delete;

    ColorDepthPass & operator=(ColorDepthPass &&) = delete;
    ColorDepthPass & operator=(ColorDepthPass const &) = delete;

private:
    Renderer const &_renderer;

    vkColorDepthPass            *_render_pass;
    vkPipeline                  *_pipeline;
    std::vector<vkFramebuffer *> _framebuffers;

    vkCmdBuffer const *_cmd_buffer;

    void _create_pipeline();
    void _create_framebuffers();
};

} // namespace btx

#endif // BRASSTACKS_RENDERING_PASSES_COLORDEPTHPASS_HPP