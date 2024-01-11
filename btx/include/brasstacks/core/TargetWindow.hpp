#ifndef BRASSTACKS_CORE_TARGETWINDOW_HPP
#define BRASSTACKS_CORE_TARGETWINDOW_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/config/RenderConfig.hpp"

namespace btx {

class TargetWindow {
public:
    explicit TargetWindow(std::string_view const app_name);
    ~TargetWindow();

    inline void show_window() { ::glfwShowWindow(_window); }
    inline void hide_window() { ::glfwHideWindow(_window); }

    void update();

#if defined(BTX_LINUX)
    auto native() const { return ::glfwGetX11Window(_window); }
#elif defined(BTX_WINDOWS)
    auto native() const { return ::glfwGetWin32Window(_window); }
#endif // BTX platform

    TargetWindow() = delete;

    TargetWindow(TargetWindow &&) = delete;
    TargetWindow(TargetWindow const &) = delete;

    TargetWindow & operator=(TargetWindow &&) = delete;
    TargetWindow & operator=(TargetWindow const &) = delete;

private:
    GLFWwindow *_window;

    void _set_window_dimensions();
};

} // namespace btx

#endif // BRASSTACKS_CORE_TARGETWINDOW_HPP