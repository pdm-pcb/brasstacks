#ifndef BRASSTACKS_PLATFORM_WIN32_WIN32CONFIGWINDOW_HPP
#define BRASSTACKS_PLATFORM_WIN32_WIN32CONFIGWINDOW_HPP

#include "brasstacks/Application/ConfigWindow.hpp"

#include <windef.h>

#include <vector>
#include <string>

namespace btx {

class Win32ConfigWindow : public ConfigWindow {
public:
    void run() override;

    Win32ConfigWindow();
    ~Win32ConfigWindow() = default;

    Win32ConfigWindow(const Win32ConfigWindow &&) = delete;
    Win32ConfigWindow(Win32ConfigWindow &)        = delete;

    Win32ConfigWindow& operator=(const Win32ConfigWindow &&) = delete;
    Win32ConfigWindow& operator=(Win32ConfigWindow &)        = delete;

private:
    ::HWND   _window;
    ::LPCSTR _classname;
    ::HDC    _device;

    ::HWND _gpu_list_label;
    ::HWND _gpu_list;
    ::HWND _res_list_label;
    ::HWND _res_list;

    ::HWND _gpu_label;
    ::HWND _chosen_gpu;
    ::HWND _res_label;
    ::HWND _chosen_res;
    ::HWND _vsync;
    ::HWND _fullscreen;

    ::HWND _api_combobox;

    ::HWND _start;
    ::HWND _quit;

    ::HICON _icon;

    char _liststr[64];

    std::vector<std::string>                   _adapters;
    std::vector<ConfigWindow::DisplaySettings> _display_settings;

    void _populate_ui();
    void _populate_gpu_list();
    void _populate_res_list();
    void _populate_api_list();
    void _set_defaults();

    void _select_gpu(::LRESULT selection);
    void _select_res(::LRESULT selection);
    void _select_api(::LRESULT selection);

    static ::LRESULT CALLBACK _wndproc(::HWND window, ::UINT msg,
                                       ::WPARAM wparam, ::LPARAM lparam);
    ::LRESULT _message_handler(::HWND window, ::UINT msg, ::WPARAM wparam,
                               ::LPARAM lparam);
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_WIN32_WIN32CONFIGWINDOW_HPP