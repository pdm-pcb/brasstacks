#ifndef DEMO_HPP
#define DEMO_HPP

#include "brasstacks/core/Application.hpp"

namespace btx {
    class vkDevice;
    class vkSwapchain;
    class vkCmdBuffer;

    class ColorDepthPass;
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

    void destroy_swapchain_resources() override;
    void create_swapchain_resources(btx::vkSwapchain const &swapchain) override;

private:
    btx::vkDevice    const *_device;
    btx::vkSwapchain const *_swapchain;

    btx::ColorDepthPass *_color_depth_pass;
};

#endif // DEMO_HPP