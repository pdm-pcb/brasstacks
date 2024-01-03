#ifndef BRASSTACKS_SYSTEM_DEBUGOVERLAY_HPP
#define BRASSTACKS_SYSTEM_DEBUGOVERLAY_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class TargetWindow;
class vkDevice;
class vkSwapchain;
class vkDescriptorPool;
class vkCmdBuffer;
class vkDebugOverlayPass;
class vkFramebuffer;

class DebugOverlay {
public:
    DebugOverlay(vkDevice const &device, TargetWindow &target_window,
                 vkSwapchain const &swapchain);
    ~DebugOverlay();

    void render_ui(vkCmdBuffer const &cmd_buffer, uint32_t const image_index);

    DebugOverlay() = delete;

    DebugOverlay(DebugOverlay &&) = delete;
    DebugOverlay(DebugOverlay const &) = delete;

    DebugOverlay & operator=(DebugOverlay &&) = delete;
    DebugOverlay & operator=(DebugOverlay const &) = delete;

private:
    vkDevice const &_device;

    TargetWindow &_target_window;

    vkDescriptorPool *_descriptor_pool;
    std::vector<vkFramebuffer *> _framebuffers;

    vkDebugOverlayPass *_overlay_pass;

    void _allocate_descriptor_pool();
    void _create_framebuffers(vkSwapchain const &swapchain);
    void _init_imgui(TargetWindow const &target_window,
                     vkSwapchain const &swapchain);

    void _draw_title_bar();
    void _draw_menu();
};

} // namespace btx

#endif // BRASSTACKS_SYSTEM_DEBUGOVERLAY_HPP