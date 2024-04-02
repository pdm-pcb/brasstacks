#ifndef BRASSTACKS_PLATFORM_IMGUI_UIOVERLAY_HPP
#define BRASSTACKS_PLATFORM_IMGUI_UIOVERLAY_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorPool.hpp"

namespace btx {

class vkColorDepthPass;

class UIOverlay {
public:
    static void init_window(::GLFWwindow *window,
                            std::string_view const window_title);
    static void create_descriptor_pool();
    static void create_swapchain_resources(vkColorDepthPass const &render_pass);
    static void destroy_swapchain_resources();
    static void destroy_descriptor_pool();
    static void shutdown_window();

    static void record_commands();
    static void render();

    static void set_enabled(bool const enabled) { _enabled = enabled; }

    static inline bool key_callback_handled() {
        return _io->WantCaptureKeyboard;
    }

    static inline bool mouse_button_callback_handled() {
        return _io->WantCaptureMouse;
    }

    UIOverlay() = delete;
    ~UIOverlay() = delete;

    UIOverlay(UIOverlay &&) = delete;
    UIOverlay(UIOverlay const &) = delete;

    UIOverlay & operator=(UIOverlay &&) = delete;
    UIOverlay & operator=(UIOverlay const &) = delete;

private:
    static ::ImGuiIO *_io;
    static ::ImGuiStyle *_style;

    static std::string _window_title;

    static std::unique_ptr<vkDescriptorPool> _descriptor_pool;

    static bool _enabled;

    static void _draw_menu_bar();
    static void _draw_status_bar();
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_IMGUI_UIOVERLAY_HPP