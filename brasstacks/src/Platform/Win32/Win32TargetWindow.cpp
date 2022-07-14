#include "brasstacks/System/pch.hpp"
#include "brasstacks/Platform/Win32/Win32TargetWindow.hpp"

#include "brasstacks/Engine/RenderConfig.hpp"
#include "brasstacks/Platform/Win32/Win32ToBTXKeys.hpp"

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

void Win32TargetWindow::init() {
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

    _client_center.x = center_x;
    _client_center.y = center_y;

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

void Win32TargetWindow::shutdown() {
    ::DestroyWindow(_window);
    ::UnregisterClass(_classname, 0);
}

void Win32TargetWindow::_register_input() {
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

        case WM_INPUT: 
        {
            // Check message size
            ::UINT dwSize;
            ::GetRawInputData(
                (::HRAWINPUT) lparam,
                RID_INPUT,
                nullptr,
                &dwSize,
                sizeof(::RAWINPUTHEADER)
            );

            // Get actual message
            ::GetRawInputData(
                (::HRAWINPUT) lparam,
                RID_INPUT,
                _raw_message,
                &dwSize,
                sizeof(::RAWINPUTHEADER)
            );

            //------------------------------------------------------------------
            // Thanks to Stefan Reinalter for much of the following code
            // https://blog.molecular-matters.com/2011/09/05/properly-handling-keyboard-input/
            //
            ::RAWINPUT *input = (::RAWINPUT *)_raw_message;

            switch(input->header.dwType) {
                case RIM_TYPEKEYBOARD:
                {                
                    ::UINT vkey  = input->data.keyboard.VKey;
                    ::UINT code  = input->data.keyboard.MakeCode;
                    ::UINT flags = input->data.keyboard.Flags;                    
                    
                    // discard "fake keys" which are part of an escaped sequence
                    if(vkey == 255) break;

                    // correct left-hand / right-hand SHIFT
                    else if(vkey == VK_SHIFT) {
                        vkey = MapVirtualKey(code, MAPVK_VSC_TO_VK_EX);
                    }

                    // correct PAUSE/BREAK and NUM LOCK silliness, and set
                    // the extended bit
                    else if(vkey == VK_NUMLOCK) {
                        code = (MapVirtualKey(vkey, MAPVK_VK_TO_VSC) | 0x100);
                    }

                    // exit condition
                    else if(vkey == VK_ESCAPE) {
                        WindowCloseEvent event;
                        publish(EventType::WindowClosed, event);
                        ::SendMessage(window, WM_CLOSE, wparam, lparam);
                        return 0;
                    }

                    // e0 and e1 are escape sequences used for certain special
                    // keys, such as PRINT and PAUSE/BREAK.
                    // see http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
                    const bool isE0 = ((flags & RI_KEY_E0) != 0);
                    const bool isE1 = ((flags & RI_KEY_E1) != 0);
                    
                    if(isE1) {
                        // for escaped sequences, turn the virtual key into the
                        // correct scan code using MapVirtualKey.
                        // however, MapVirtualKey is unable to map VK_PAUSE
                        // (this is a known bug), hence we map that by hand.
                        if(vkey == VK_PAUSE) {
                            code = 0x45;
                        }
                        else {
                            code = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
                        }
                    }

                    switch(vkey) {
                        // right-hand CONTROL and ALT have their e0 bit set
                        case VK_CONTROL:
                            if(isE0) vkey = KB_RCTRL;
                            else     vkey = KB_LCTRL;
                            break;
                        
                        case VK_MENU:
                            if(isE0) vkey = KB_RALT;
                            else     vkey = KB_LALT;
                            break;
                        
                        // NUMPAD ENTER has its e0 bit set
                        case VK_RETURN: if(isE0) vkey = KB_NP_ENTER; break;
                        
                        // the standard INSERT, DELETE, HOME, END, PRIOR and
                        // NEXT keys will always have their e0 bit set, but the
                        // corresponding keys on the NUMPAD will not.
                        case VK_INSERT: if(!isE0) vkey = KB_NP_0;       break;
                        case VK_DELETE: if(!isE0) vkey = KB_NP_DECIMAL; break;
                        case VK_HOME:   if(!isE0) vkey = KB_NP_7;       break;
                        case VK_END:    if(!isE0) vkey = KB_NP_1;       break;
                        case VK_PRIOR:  if(!isE0) vkey = KB_NP_9;       break;
                        case VK_NEXT:   if(!isE0) vkey = KB_NP_3;       break;
                        
                        // the standard arrow keys will always have their e0 bit
                        // set, but the corresponding keys on the NUMPAD will
                        // not.
                        case VK_LEFT:  if(!isE0) vkey = KB_NP_4; break;
                        case VK_RIGHT: if(!isE0) vkey = KB_NP_6; break;
                        case VK_UP:    if(!isE0) vkey = KB_NP_8; break;
                        case VK_DOWN:  if(!isE0) vkey = KB_NP_2; break;
                        
                        // NUMPAD 5 doesn't have its e0 bit set
                        case VK_CLEAR: if(!isE0) vkey = KB_NP_5; break;
                    }

                    // a key can either produce a "make" or "break" scancode.
                    // this is used to differentiate between down-presses and
                    // releases
                    // see http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
                    const bool was_release = ((flags & RI_KEY_BREAK) != 0);
                    
                    // getting a human-readable string
                    // ::UINT key = (code << 16) | (isE0 << 24);
                    // char buffer[512] = {};
                    // ::GetKeyNameText((::LONG)key, buffer, 512);
                    // PDR_ENGINE_TRACE("{}, {}, {}", vkey, code, buffer);

                    if(was_release) {
                        KeyReleasedEvent event(win32_to_btx(vkey));
                        publish(EventType::KeyReleased, event);
                    }
                    else {
                        KeyPressedEvent event(win32_to_btx(vkey));
                        publish(EventType::KeyPressed, event);
                    }

                    break;
                }
                case RIM_TYPEMOUSE:
                {
                    ::SetCursorPos(_client_center.x, _client_center.y);
                    ::SetCursor(nullptr);

                    ::RAWMOUSE mouse = input->data.mouse;

                    if(mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) {
                        MouseButtonPressedEvent event(MOUSE_BUTTON_LEFT);
                        publish(EventType::MouseButtonPressed, event);
                    }
                    else if(mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP) {
                        MouseButtonReleasedEvent event(MOUSE_BUTTON_LEFT);
                        publish(EventType::MouseButtonPressed, event);
                    }
                    else if(mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) {
                        MouseButtonPressedEvent event(MOUSE_BUTTON_RIGHT);
                        publish(EventType::MouseButtonReleased, event);
                    }
                    else if(mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP) {
                        MouseButtonReleasedEvent event(MOUSE_BUTTON_RIGHT);
                        publish(EventType::MouseButtonReleased, event);
                    }
                    else if(mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN) {
                        MouseButtonPressedEvent event(MOUSE_BUTTON_MIDDLE);
                        publish(EventType::MouseButtonPressed, event);
                    }
                    else if(mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP) {
                        MouseButtonReleasedEvent event(MOUSE_BUTTON_MIDDLE);
                        publish(EventType::MouseButtonReleased, event);
                    }
                    else if(mouse.usButtonFlags & RI_MOUSE_WHEEL) {
                        MouseScrolledEvent event(0, mouse.usButtonData);
                        publish(EventType::MouseScrolled, event);
                    }
                    else if(mouse.usButtonFlags & RI_MOUSE_HWHEEL) {
                        MouseScrolledEvent event(mouse.usButtonData, 0);
                        publish(EventType::MouseScrolled, event);
                    }

                    if(mouse.lLastX != 0 || mouse.lLastY != 0) {
                        MouseMovedEvent event (mouse.lLastX, mouse.lLastY);
                        publish(EventType::MouseMoved, event);
                    }

                    break;
                }
                break;
            }
            break;
        }

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
    _window        { nullptr },
    _classname     { "Win32TargetWindow" },
    _raw_message   { new ::BYTE[64] }, 
    _client_center { 0, 0 }
{ }

Win32TargetWindow::~Win32TargetWindow() {
    delete _raw_message;
}

} // namespace btx