#include "brasstacks/System/pch.hpp"
#include "brasstacks/Platform/Win32/Win32TargetWindow.hpp"

#include "brasstacks/Engine/RenderConfig.hpp"

namespace btx {

void Win32TargetWindow::run() {
    ::ShowWindow(_window, SW_SHOWNORMAL);

    ::MSG msg { };
    while(::GetMessage(&msg, nullptr, 0, 0) > 0)
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
}

void Win32TargetWindow::_register_input() {
    _raw_message = new ::BYTE[64];

    if(_raw_message == nullptr) {
        ::MessageBox(nullptr, "Unable allocate lpbyte", "Error", MB_OK);
    }

    ::RAWINPUTDEVICE devices[2];

    devices[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
    devices[0].usUsage     = HID_USAGE_GENERIC_KEYBOARD;
    devices[0].dwFlags     = 0; // RIDEV_NOLEGACY ?
    devices[0].hwndTarget  = _window;

    devices[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
    devices[1].usUsage     = HID_USAGE_GENERIC_MOUSE;
    devices[1].dwFlags     = RIDEV_NOLEGACY;
    devices[1].hwndTarget  = _window;

    if(!::RegisterRawInputDevices(devices, 2, sizeof(::RAWINPUTDEVICE))) {
        ::MessageBox(
            nullptr, "Could not register for raw HID input:",
            "Error", MB_OK    
        );
    }
    else {
        BTX_ENGINE_TRACE("Registered for raw win32 input");
    }
}

::LRESULT CALLBACK Win32TargetWindow::_wndproc(::HWND window,
                                               ::UINT msg,
                                               ::WPARAM wparam,
                                               ::LPARAM lparam) {
    Win32TargetWindow *_this = nullptr;

    if(msg == WM_NCCREATE) {
        ::LPCREATESTRUCT lpstruct = reinterpret_cast<::LPCREATESTRUCT>(lparam);
        _this = static_cast<Win32TargetWindow *>(lpstruct->lpCreateParams);

        ::SetWindowLongPtr(
            window,
            GWLP_USERDATA,
            reinterpret_cast<::LONG_PTR>(_this)
        );

        _this->_window = window;
    }
    else {
        _this = reinterpret_cast<Win32TargetWindow *>(
            ::GetWindowLongPtr(window, GWLP_USERDATA)
        );
    }

    if(_this) {
        return _this->_message_handler(window, msg, wparam, lparam);
    }
    else {
        return DefWindowProc(window, msg, wparam, lparam);
    }
}

::LRESULT Win32TargetWindow::_message_handler(::HWND window,
                                              ::UINT msg,
                                              ::WPARAM wparam,
                                              ::LPARAM lparam) {
    switch(msg) {
        case WM_SETCURSOR: 
            ::SetCursor(nullptr);
            break;

        case WM_KEYDOWN:
            if(wparam == VK_ESCAPE) {
                BTX_ENGINE_TRACE("Keyboard exit request received");
                ::SendMessage(window, WM_CLOSE, wparam, lparam);
            }
            break;

        case WM_CLOSE:
            ::PostQuitMessage(0);
            return 0;

        case WM_DESTROY:
            return 0;

        default: break;
    }

    return ::DefWindowProc(window, msg, wparam, lparam);
}

Win32TargetWindow::Win32TargetWindow() :
    _window      { nullptr },
    _classname   { "Win32TargetWindow" },
    _raw_message { nullptr }
{
    BTX_ENGINE_TRACE(
        "Creating Win32 target window {}x{}",
        RenderConfig::window_x_res,
        RenderConfig::window_y_res
    );

    ::WNDCLASSEX wcex { };
    wcex.cbSize = sizeof(::WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = _wndproc;
    wcex.hbrBackground = static_cast<::HBRUSH>(GetStockObject(BLACK_BRUSH));
    wcex.lpszClassName = _classname;

    if(::RegisterClassEx(&wcex) == 0) {
        ::MessageBox(nullptr, "RegisterClassEx() failed", "Error", MB_OK);
    }

    _window = ::CreateWindowExA(
        0, _classname, "Brass Tacks Engine",
        WS_POPUP | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        static_cast<int>(RenderConfig::window_x_res),
        static_cast<int>(RenderConfig::window_y_res),
        nullptr, nullptr, 0, this
    );

    if(_window == nullptr) {
        ::MessageBox(nullptr, "CreateWindowEx() failed", "Error", MB_OK);
    }

    int monitor_x = ::GetSystemMetrics(SM_CXSCREEN);
    int monitor_y = ::GetSystemMetrics(SM_CYSCREEN);

    int center_x = (monitor_x / 2) - (RenderConfig::window_x_res / 2);
    int center_y = (monitor_y / 2) - (RenderConfig::window_y_res / 2);

    ::SetWindowPos(
        _window, nullptr,
        center_x, center_y,
        static_cast<int>(RenderConfig::window_x_res),
        static_cast<int>(RenderConfig::window_y_res),
        0
    );
    ::SetCapture(_window);
    ::SetCursorPos(center_x, center_y);
    ::SetCursor(nullptr);
    ::ShowCursor(false);

    _register_input();
}

Win32TargetWindow::~Win32TargetWindow() {
    ::DestroyWindow(_window);
    ::UnregisterClass(_classname, 0);

    delete _raw_message;
}

} // namespace btx