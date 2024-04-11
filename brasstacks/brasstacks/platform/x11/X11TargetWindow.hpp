#ifdef BTX_UNUSED

#ifndef BRASSTACKS_PLATFORM_X11_X11TARGETWINDOW_HPP
#define BRASSTACKS_PLATFORM_X11_X11TARGETWINDOW_HPP

#ifdef BTX_LINUX

#include "brasstacks/pch.hpp"

#include "brasstacks/config/RenderConfig.hpp"
#include "brasstacks/platform/x11/X11MsgToStr.hpp"
#include "brasstacks/platform/x11/X11ToBTXKeys.hpp"

namespace btx {

class X11TargetWindow final {
public:
    explicit X11TargetWindow(std::string_view const app_name);
    ~X11TargetWindow();

    void start();
    void stop();
    void run();

    void toggle_cursor_capture();

    inline auto const & native() const { return _window; }
    inline auto const & display() const { return _display; }

    inline RenderConfig::Size const size() {
        std::unique_lock<std::mutex> read_lock(_size_mutex);
        return { .width = _window_size.width, .height = _window_size.height };
    }

    inline RenderConfig::Offset const posiotion() {
        std::unique_lock<std::mutex> read_lock(_position_mutex);
        return { .x = _window_position.x, .y = _window_position.y };
    }

    X11TargetWindow() = delete;

    X11TargetWindow(X11TargetWindow &&) = delete;
    X11TargetWindow(X11TargetWindow const &) = delete;

    X11TargetWindow & operator=(X11TargetWindow &&) = delete;
    X11TargetWindow & operator=(X11TargetWindow const &) = delete;

private:
    // x11 specifics
    ::Display *_display;
    ::Window  _root;
    ::Window  _window;

    ::XVisualInfo          _visual_info;
    ::XSetWindowAttributes _attributes;

    std::string _window_title;

    // std::byte *_raw_msg;

    // For message loop debugging
    static X11MsgToStr const _msg_map;

    // For brasstacks/x11 interop
    static X11ToBTXKeys const _keymap;

    // Details for later
    RenderConfig::Size   _screen_size;
    RenderConfig::Offset _screen_center;

    RenderConfig::Size   _window_size;
    RenderConfig::Offset _window_position;

    std::mutex _size_mutex;
    std::mutex _position_mutex;

    bool _minimized;

    std::atomic_flag _run_flag;

    std::atomic<bool> _toggle_cursor_capture;
    bool _cursor_captured;

    void _create_window();
    void _destroy_window();
    void _register_raw_input();
    static void _deregister_raw_input();
    void _restrict_cursor();
    void _release_cursor();
    void _size_and_place();

    void _message_loop();

    // // Static wndproc for Windows to call, per the Raymond Chen article:
    // // https://devblogs.microsoft.com/oldnewthing/20140203-00/?p=1893
    // static ::LRESULT CALLBACK _static_wndproc(::HWND hWnd, ::UINT uMsg,
    //                                           ::WPARAM wParam, ::LPARAM lParam);

    // // Per-instance wndproc to actually handle the OS messages
    // ::LRESULT CALLBACK _inst_wndproc(::HWND hWnd, ::UINT uMsg,
    //                                  ::WPARAM wParam, ::LPARAM lParam);

    // static void _parse_raw_keyboard(::RAWKEYBOARD const &raw);
    // static void _parse_raw_mouse(::RAWMOUSE const &raw);
};

} // namespace btx

#endif // BTX_LINUX

#endif // BRASSTACKS_PLATFORM_X11_X11TARGETWINDOW_HPP

#endif // BTX_UNUSED