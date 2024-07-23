#ifndef BRASSTACKS_CORE_TARGETWINDOW_HPP
#define BRASSTACKS_CORE_TARGETWINDOW_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/platform/vulkan/swapchain/vkSurface.hpp"

#ifdef BTX_LINUX
    #define GLFW_EXPOSE_NATIVE_X11
#elif BTX_WINDOWS
    #define GLFW_EXPOSE_NATIVE_WIN32
#endif // BTX platform

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace btx {

class TargetWindow final {
public:
    static void init(std::string_view const app_name);
    static void shutdown();

    static void create_surface();
    static void destroy_surface();

    static inline void show() { ::glfwShowWindow(_window); }
    static inline void hide() { ::glfwHideWindow(_window); }

    [[nodiscard]] static bool poll_events();

    [[nodiscard]] static auto const &surface() { return _surface; }

    TargetWindow() = delete;
    ~TargetWindow() = delete;

    TargetWindow(TargetWindow &&) = delete;
    TargetWindow(TargetWindow const &) = delete;

    TargetWindow & operator=(TargetWindow &&) = delete;
    TargetWindow & operator=(TargetWindow const &) = delete;

private:
    static GLFWwindow *_window;

    static Offset _screen_center;
    static Size   _window_size;
    static Offset _window_position;

    static vkSurface _surface;

    static void _get_resolution();
    static void _size_and_place();

    static void _error_callback(int code, char const *message);

    static void _key_callback(GLFWwindow *window, int key, int scancode,
                              int action, int mods);
    static void _mouse_move_callback(GLFWwindow* window, double x, double y);
    static void _mouse_button_callback(GLFWwindow* window, int button,
                                       int action, int mods);
    static void _window_iconify_callback(GLFWwindow* window, int iconified);

};

} // namespace btx

#endif // BRASSTACKS_CORE_TARGETWINDOW_HPP