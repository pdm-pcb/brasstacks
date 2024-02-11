#ifndef BRASSTACKS_CORE_TARGETWINDOW_HPP
#define BRASSTACKS_CORE_TARGETWINDOW_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/config/RenderConfig.hpp"
#include "brasstacks/platform/x11/X11TargetWindow.hpp"
#include "brasstacks/platform/win32/Win32TargetWindow.hpp"

namespace btx {

#ifdef BTX_LINUX
    using TargetWindow = X11TargetWindow;
#elif BTX_WINDOWS
    using TargetWindow = Win32TargetWindow;
#endif // BTX platform

// class TargetWindow final {
// public:
//     explicit TargetWindow(std::string_view const app_name);
//     ~TargetWindow();

//     inline void show_window() { ::glfwShowWindow(_window); }
//     inline void hide_window() { ::glfwHideWindow(_window); }

//     void poll_events();

// #ifdef BTX_LINUX
//     auto native() const { return ::glfwGetX11Window(_window); }
// #elif BTX_WINDOWS
//     auto native() const { return ::glfwGetWin32Window(_window); }
// #endif // BTX platform

//     TargetWindow() = delete;

//     TargetWindow(TargetWindow &&) = delete;
//     TargetWindow(TargetWindow const &) = delete;

//     TargetWindow & operator=(TargetWindow &&) = delete;
//     TargetWindow & operator=(TargetWindow const &) = delete;

// private:
//     GLFWwindow *_window;

//     void _set_window_dimensions();
// };

} // namespace btx

#endif // BRASSTACKS_CORE_TARGETWINDOW_HPP