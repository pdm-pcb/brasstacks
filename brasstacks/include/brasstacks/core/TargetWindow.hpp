#ifndef BRASSTACKS_CORE_TARGETWINDOW_HPP
#define BRASSTACKS_CORE_TARGETWINDOW_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/config/RenderConfig.hpp"

namespace btx {

class TargetWindow final {
public:
    explicit TargetWindow(std::string_view const app_name);
    ~TargetWindow();

    inline void show() { ::glfwShowWindow(_window); }
    inline void hide() { ::glfwHideWindow(_window); }

    void capture_mouse() const;
    void release_mouse() const;

    void poll_events();

#ifdef BTX_LINUX
    inline auto native() const { return ::glfwGetX11Window(_window); }
    inline auto * display() const { return ::glfwGetX11Display(); }
#elif BTX_WINDOWS
    inline auto * native() const { return ::glfwGetWin32Window(_window); }
#endif // BTX platform

    TargetWindow() = delete;

    TargetWindow(TargetWindow &&) = delete;
    TargetWindow(TargetWindow const &) = delete;

    TargetWindow & operator=(TargetWindow &&) = delete;
    TargetWindow & operator=(TargetWindow const &) = delete;

private:
    GLFWwindow *_window;

    RenderConfig::Size   _screen_size;
    RenderConfig::Offset _screen_center;

    RenderConfig::Size   _window_size;
    RenderConfig::Offset _window_position;

    static double _last_cursor_x;
    static double _last_cursor_y;

    void _calc_window_dimensions();
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