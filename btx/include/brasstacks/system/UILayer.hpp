#ifndef BRASSTACKS_SYSTEM_UILAYER_HPP
#define BRASSTACKS_SYSTEM_UILAYER_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class TargetWindow;
class vkDevice;
class vkSwapchain;
class vkDescriptorPool;
class vkCmdBuffer;
class vkUILayerPass;
class vkFramebuffer;

class UILayer {
public:
    UILayer(vkDevice const &device, TargetWindow &target_window,
            vkSwapchain const &swapchain);
    ~UILayer();

    void render_ui(vkCmdBuffer const &cmd_buffer, uint32_t const image_index);

    UILayer() = delete;

    UILayer(UILayer &&) = delete;
    UILayer(UILayer const &) = delete;

    UILayer & operator=(UILayer &&) = delete;
    UILayer & operator=(UILayer const &) = delete;

private:
    vkDevice const &_device;

    TargetWindow &_target_window;

    vkDescriptorPool *_descriptor_pool;
    std::vector<vkFramebuffer *> _framebuffers;

    vkUILayerPass *_overlay_pass;

    void _allocate_descriptor_pool();
    void _create_framebuffers(vkSwapchain const &swapchain);
    void _init_imgui(TargetWindow const &target_window,
                     vkSwapchain const &swapchain);

    void _draw_title_bar();
    void _draw_menu();
};

} // namespace btx

#endif // BRASSTACKS_SYSTEM_UILAYER_HPP