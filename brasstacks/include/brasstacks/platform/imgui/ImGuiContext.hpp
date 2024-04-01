#ifndef BRASSTACKS_PLATFORM_IMGUI_IMGUICONTEXT_HPP
#define BRASSTACKS_PLATFORM_IMGUI_IMGUICONTEXT_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorPool.hpp"

namespace btx {

class vkColorDepthPass;
class vkCmdBuffer;

class ImGuiContext {
public:
    static void init_window(::GLFWwindow *window);
    static void create_descriptor_pool();
    static void create_swapchain_resources(vkColorDepthPass const &render_pass);
    static void destroy_swapchain_resources();
    static void destroy_descriptor_pool();
    static void shutdown_window();

    static void record_commands();
    static void render(vkCmdBuffer const &cmd_buffer);

    static void set_enabled(bool const enabled) { _enabled = enabled; }

    static inline bool key_callback_handled() {
        return _io->WantCaptureKeyboard;
    }

    static inline bool mouse_button_callback_handled() {
        return _io->WantCaptureMouse;
    }

    ImGuiContext() = delete;
    ~ImGuiContext() = delete;

    ImGuiContext(ImGuiContext &&) = delete;
    ImGuiContext(ImGuiContext const &) = delete;

    ImGuiContext & operator=(ImGuiContext &&) = delete;
    ImGuiContext & operator=(ImGuiContext const &) = delete;

private:
    static ::ImGuiIO *_io;
    static ::ImGuiStyle *_style;

    static std::unique_ptr<vkDescriptorPool> _descriptor_pool;

    static bool _enabled;

    static void _draw_menu_bar();
    static void _draw_perf_window();
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_IMGUI_IMGUICONTEXT_HPP