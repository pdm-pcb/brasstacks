#ifndef BRASSTACKS_PLATFORM_WIN32_WIN32TARGETWINDOW_HPP
#define BRASSTACKS_PLATFORM_WIN32_WIN32TARGETWINDOW_HPP

#include "brasstacks/Engine/TargetWindow.hpp"

namespace btx {

class Win32TargetWindow : public TargetWindow {
public:
    void run() override;

    void * get_native() const override { return static_cast<void *>(_window); }

    Win32TargetWindow();
    ~Win32TargetWindow() override;

    Win32TargetWindow(const Win32TargetWindow &&) = delete;
    Win32TargetWindow(Win32TargetWindow &)        = delete;

    Win32TargetWindow& operator=(const Win32TargetWindow &&) = delete;
    Win32TargetWindow& operator=(Win32TargetWindow &)        = delete;

private:
    ::HWND   _window;
    ::LPCSTR _classname;
    ::LPBYTE _raw_message;

    void _register_input();

    static ::LRESULT CALLBACK _wndproc(::HWND window, ::UINT msg,
                                       ::WPARAM wparam, ::LPARAM lparam);
    ::LRESULT _message_handler(::HWND window, ::UINT msg, ::WPARAM wparam,
                               ::LPARAM lparam);
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_WIN32_WIN32TARGETWINDOW_HPP