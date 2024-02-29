#ifdef BTX_UNUSED

#ifndef BRASSTACKS_PLATFORM_WIN32_WIN32TARGETWINDOW_HPP
#define BRASSTACKS_PLATFORM_WIN32_WIN32TARGETWINDOW_HPP

#ifdef BTX_WINDOWS

#include "brasstacks/pch.hpp"

#include "brasstacks/config/RenderConfig.hpp"
#include "brasstacks/platform/win32/Win32MsgToStr.hpp"
#include "brasstacks/platform/win32/Win32ToBTXKeys.hpp"

namespace btx {

class Win32TargetWindow final {
public:
    explicit Win32TargetWindow(std::string_view const app_name);
    ~Win32TargetWindow();

    void show();
    void hide();

    void poll_events();

    inline auto const & native() const { return _window_handle; }

    inline RenderConfig::Size const size() {
        std::unique_lock<std::mutex> read_lock(_size_mutex);
        return { .width = _window_size.width, .height = _window_size.height };
    }

    inline RenderConfig::Offset const posiotion() {
        std::unique_lock<std::mutex> read_lock(_position_mutex);
        return { .x = _window_position.x, .y = _window_position.y };
    }

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
    RenderConfig::Size   _screen_size;
    RenderConfig::Offset _screen_center;

    RenderConfig::Size   _window_size;
    RenderConfig::Offset _window_position;

    std::mutex _size_mutex;
    std::mutex _position_mutex;

    bool _minimized;

    void _register_class();
    void _create_window();
    void _destroy_window();
    void _size_and_place();
    void _register_raw_input();
    static void _deregister_raw_input();
    void _restrict_cursor();
    void _release_cursor();

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

#endif // BTX_UNUSED