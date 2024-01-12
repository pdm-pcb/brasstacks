#ifndef BRASSTACKS_PLATFORM_WIN32_WIN32TARGETWINDOW_HPP
#define BRASSTACKS_PLATFORM_WIN32_WIN32TARGETWINDOW_HPP

#ifdef BTX_WINDOWS

#include "brasstacks/pch.hpp"

#include "brasstacks/platform/win32/Win32MsgToStr.hpp"
#include "brasstacks/platform/win32/Win32ToBTXKeys.hpp"

namespace btx {

class Win32TargetWindow final {
public:
    explicit Win32TargetWindow(std::string_view const app_name);
    ~Win32TargetWindow();

    void show_window();
    void hide_window();

    void poll_events();

    ::HWND const & native() const { return _window_handle; }

    Win32TargetWindow() = delete;

    Win32TargetWindow(Win32TargetWindow &&) = delete;
    Win32TargetWindow(Win32TargetWindow const &) = delete;

    Win32TargetWindow & operator=(Win32TargetWindow &&) = delete;
    Win32TargetWindow & operator=(Win32TargetWindow const &) = delete;

private:
    // win32 specifics
    std::wstring  _class_name;
    ::WNDCLASSEXW _window_class;
    std::wstring  _window_title;
    ::HWND        _window_handle;

    std::byte *_raw_msg;

    // For message loop debugging
    static Win32MsgToStr const _msg_map;

    // For brasstacks/win32 interop
    static Win32ToBTXKeys const _keymap;

    // Details for later
    RenderConfig::Dimensions _screen_size;
    RenderConfig::Offset    _screen_center;

    bool _minimized;
    bool _running;

    void _register_class();
    void _create_window();
    void _destroy_window();
    void _register_raw_input();
    static void _deregister_raw_input();
    void _restrict_cursor();
    void _release_cursor();
    void _size_and_place();

    // Static wndproc for Windows to call, per the Raymond Chen article:
    // https://devblogs.microsoft.com/oldnewthing/20140203-00/?p=1893
    static ::LRESULT CALLBACK _static_wndproc(::HWND hWnd, ::UINT uMsg,
                                              ::WPARAM wParam, ::LPARAM lParam);

    // Per-instance wndproc to actually handle the OS messages
    ::LRESULT CALLBACK _inst_wndproc(::HWND hWnd, ::UINT uMsg,
                                     ::WPARAM wParam, ::LPARAM lParam);

    static void _parse_raw_keyboard(::RAWKEYBOARD const &raw);
    static void _parse_raw_mouse(::RAWMOUSE const &raw);
};

} // namespace btx

#endif // BTX_WINDOWS

#endif // BRASSTACKS_PLATFORM_WIN32_WIN32TARGETWINDOW_HPP