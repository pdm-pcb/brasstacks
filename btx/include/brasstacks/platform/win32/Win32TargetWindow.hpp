#ifndef BRASSTACKS_PLATFORM_WIN32_WIN32TARGETWINDOW_HPP
#define BRASSTACKS_PLATFORM_WIN32_WIN32TARGETWINDOW_HPP

#include "brasstacks/pch.hpp"

#include "brasstacks/platform/win32/Win32ToBTXKeys.hpp"

namespace btx {

class vkInstance;

class Win32TargetWindow final {
public:
    struct Dimensions {
        uint32_t width = 0;
        uint32_t height = 0;
    };

    struct Position {
        int32_t x = 0;
        int32_t y = 0;
    };

    static void init(std::string_view const app_name);
    static void shutdown();

    // Size, place, and make visible a native window
    static void create_window(Dimensions const &dimensions = { 0u, 0u },
                              Position const &position = { 0, 0 });
    static void show_window();
    static void destroy_window();

    // Manage the graphics surface
    static void create_surface(const vkInstance &instance);
    static void destroy_surface(const vkInstance &instance);

    // Give the OS a moment to speak up
    static void message_loop();

    inline static auto const& surface() { return _surface; }

    // Only one target window at a time
    Win32TargetWindow() = delete;

private:
    // Win32 specifics
    static ::LPCSTR _window_title;
    static ::HWND   _window;
    static ::HDC    _device;

    static char *_raw_msg;

    // Vulkan specifics
    static vk::SurfaceKHR _surface;

    static Win32ToBTXKeys const _keymap;
    static Position _screen_center;

    static void _register_input();
    static void _restrict_cursor();
    static void _release_cursor();
    static void _size_and_place();

    // The way in for Windows
    static ::LRESULT CALLBACK _wndproc(::HWND hWnd, ::UINT uMsg,
                                       ::WPARAM wParam, ::LPARAM lParam);

    static void _parse_raw_keyboard(::RAWKEYBOARD const &raw);
    static void _parse_raw_mouse(::RAWMOUSE const &raw);
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_WIN32_WIN32TARGETWINDOW_HPP