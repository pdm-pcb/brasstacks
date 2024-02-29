#ifndef BRASSTACKS_CORE_TARGETWINDOW_HPP
#define BRASSTACKS_CORE_TARGETWINDOW_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/config/RenderConfig.hpp"
#include "brasstacks/platform/input/GLFWToBTXKeys.hpp"

namespace btx {

class TargetWindow final {
public:
    explicit TargetWindow(std::string_view const app_name);
    ~TargetWindow();

    inline void show() { ::glfwShowWindow(_window); }
    inline void hide() { ::glfwHideWindow(_window); }

    void poll_events();

#ifdef BTX_LINUX
    inline auto native() const { return ::glfwGetX11Window(_window); }
#elif BTX_WINDOWS
    inline auto * native() const { return ::glfwGetWin32Window(_window); }
#endif // BTX platform

    inline auto const & size()      const { return _window_size; }
    inline auto const & posiotion() const { return _window_position; }

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

    static GLFWToBTXKeys _keymap;

    void _get_window_dimensions();
    static void _glfw_error_callback(int code, char const *message);
    static void _glfw_key_callback(GLFWwindow *window, int key, int scancode,
                                   int action, int mods);
};

} // namespace btx

#endif // BRASSTACKS_CORE_TARGETWINDOW_HPP