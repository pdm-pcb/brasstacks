#ifndef BRASSTACKS_CORE_TARGETWINDOW_HPP
#define BRASSTACKS_CORE_TARGETWINDOW_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/config/RenderConfig.hpp"

namespace btx {

class TargetWindow final {
public:
    static void init(std::string_view const app_name);
    static void shutdown();

    static inline void show() { ::glfwShowWindow(_window); }
    static inline void hide() { ::glfwHideWindow(_window); }

    static void capture_mouse();
    static void release_mouse();

    static void poll_events();

#ifdef BTX_LINUX
    static inline auto native() { return ::glfwGetX11Window(_window); }
    static inline auto * display() { return ::glfwGetX11Display(); }
#elif BTX_WINDOWS
    static inline auto * native() { return ::glfwGetWin32Window(_window); }
#endif // BTX platform

    TargetWindow() = delete;
    ~TargetWindow() = delete;

    TargetWindow(TargetWindow &&) = delete;
    TargetWindow(TargetWindow const &) = delete;

    TargetWindow & operator=(TargetWindow &&) = delete;
    TargetWindow & operator=(TargetWindow const &) = delete;

private:
    static GLFWwindow *_window;

    static RenderConfig::Size   _screen_size;
    static RenderConfig::Offset _screen_center;

    static RenderConfig::Size   _window_size;
    static RenderConfig::Offset _window_position;

    static double _last_cursor_x;
    static double _last_cursor_y;

    static void _calc_window_dimensions();

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