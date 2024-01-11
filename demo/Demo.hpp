#ifndef DEMO_HPP
#define DEMO_HPP

#include "brasstacks/core/Application.hpp"

namespace btx {
    class vkCmdBuffer;
    class vkColorPass;
    class vkPipeline;
    class vkFramebuffer;
    class vkDevice;
    class vkSwapchain;
} // namespace btx

class Demo final : public btx::Application {
public:
    Demo();
    ~Demo() override = default;

    void init(btx::vkDevice const &device,
              btx::vkSwapchain const &swapchain) override;
    void shutdown() override;

    void update() override;

    void record_commands(btx::vkCmdBuffer const &cmd_buffer,
                         uint32_t const image_index) override;

private:
    btx::vkDevice    const *_device;
    btx::vkSwapchain const *_swapchain;

    btx::vkColorPass                 *_color_pass;
    btx::vkPipeline                  *_color_pipeline;
    std::vector<btx::vkFramebuffer *> _color_framebuffers;

    void _create_color_pass();
    void _destroy_color_pass();
};

#endif // DEMO_HPP