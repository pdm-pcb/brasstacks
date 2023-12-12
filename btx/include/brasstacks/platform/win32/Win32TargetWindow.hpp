#ifndef BRASSTACKS_PLATFORM_WIN32_WIN32TARGETWINDOW_HPP
#define BRASSTACKS_PLATFORM_WIN32_WIN32TARGETWINDOW_HPP

#ifdef BTX_WINDOWS

#include "brasstacks/pch.hpp"

#include "brasstacks/system/TargetWindow.hpp"
#include "brasstacks/platform/win32/Win32ToBTXKeys.hpp"

namespace btx {

class Win32TargetWindow final : public TargetWindow {
public:
    void create_window(Dimensions const &dimensions = { 0u, 0u },
                       Position const &position = { 0, 0 }) override;
    void show_window() override;
    void destroy_window() override;

    void create_surface(vk::Instance const &instance) override;
    void destroy_surface() override;

    void message_loop() override;

    inline vk::SurfaceKHR const & surface() const override { return _surface; }

    explicit Win32TargetWindow(std::string_view const app_name);
    ~Win32TargetWindow() override;

    Win32TargetWindow() = delete;

    Win32TargetWindow(Win32TargetWindow &&) = delete;
    Win32TargetWindow(Win32TargetWindow const &) = delete;

    Win32TargetWindow & operator=(Win32TargetWindow &&) = delete;
    Win32TargetWindow & operator=(Win32TargetWindow const &) = delete;

private:
    // Win32 specifics
    ::LPCSTR _window_title;
    ::HWND   _window_handle;
    ::HDC    _device_context;

    char *_raw_msg;

    // Vulkan specifics
    vk::Instance const *_instance;
    vk::SurfaceKHR _surface;

    Win32ToBTXKeys const _keymap;
    Position _screen_center;

    void _register_input();
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

    void _parse_raw_keyboard(::RAWKEYBOARD const &raw);
    void _parse_raw_mouse(::RAWMOUSE const &raw);
};

} // namespace btx

#endif // BTX_WINDOWS

#endif // BRASSTACKS_PLATFORM_WIN32_WIN32TARGETWINDOW_HPP