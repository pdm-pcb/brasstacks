#ifdef BTX_WINDOWS

#include "brasstacks/platform/win32/Win32TargetWindow.hpp"

#include "brasstacks/events/EventBroker.hpp"
#include "brasstacks/events/keyboard_events.hpp"
#include "brasstacks/events/mouse_events.hpp"
#include "brasstacks/platform/vulkan/vkInstance.hpp"

namespace btx {

// =============================================================================
Win32TargetWindow::Win32TargetWindow(std::string_view const app_name,
                                     Dimensions const &dimensions,
                                     Position const &position) :
    _window_title   { app_name.data() },
    _window_handle  { nullptr },
    _device_context { nullptr },
    _raw_msg        { new char[sizeof(::RAWINPUT)] },
    _msg_map        { },
    _keymap         { },
    _screen_size    { 0u, 0u },
    _window_size    { dimensions },
    _screen_center  { 0, 0 },
    _window_pos     { position }
{
    // Set DPI awareness before querying for resolution
    auto const set_dpi_awareness_result =
        ::SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

    if(!SUCCEEDED(set_dpi_awareness_result)) {
        auto const error = ::GetLastError();
        BTX_WARN(
            "Could not set process DPI awareness. Error {}: '{}'",
            error,
            std::system_category().message(static_cast<int>(error))
        );
    }

    // Testing for the primary display's resolution this way feels brittle,
    // but it does work for the average use case
    _screen_size = {
        static_cast<uint32_t>(::GetSystemMetrics(SM_CXSCREEN)),
        static_cast<uint32_t>(::GetSystemMetrics(SM_CYSCREEN))
    };

    // Hold on to the screen's center for later
    _screen_center = {
        static_cast<int32_t>(_screen_size.width / 2),
        static_cast<int32_t>(_screen_size.height / 2)
    };

    _register_class();
    _create_window();
    _size_and_place();

    show_window();
}

// =============================================================================
Win32TargetWindow::~Win32TargetWindow() {
    _destroy_window();
    // Clean up our one allocation
    delete[] _raw_msg;
}

// =============================================================================
void Win32TargetWindow::show_window() {
    ::ShowWindow(_window_handle, SW_SHOWNORMAL);
}

// =============================================================================
void Win32TargetWindow::hide_window() {
    ::ShowWindow(_window_handle, SW_HIDE);
}

// =============================================================================
void Win32TargetWindow::message_loop() {
    static ::MSG message;
    ::memset(&message, 0, sizeof(::MSG));

    // Run through available messages from the OS
    while(::PeekMessage(&message, _window_handle, 0u, 0u, PM_REMOVE) != 0) {
        ::DispatchMessage(&message);
    }
}

// =============================================================================
void Win32TargetWindow::_register_class() {
    // Now it's time to register the window class
    ::WNDCLASSEXA const wcex {
        .cbSize = sizeof(::WNDCLASSEXA),

        // These flags instruct Windows to redraw the entire window surface if
        // the size changes; by default it'll just refresh the new real estate
        .style = CS_HREDRAW | CS_VREDRAW,

        // Specify the callback for Windows messages
        .lpfnWndProc = _static_wndproc,

        // These two are unused
        .cbClsExtra = 0,
        .cbWndExtra = 0,

        // hInstance is null since we're not using winmain
        .hInstance = nullptr,

        // No icon. Maybe some day?
        .hIcon = nullptr,

        // Also no custom cursor
        .hCursor = nullptr,

        // I'm setting the window's background to black so it doesn't flash
        // white before the renderer can set a clear color
        .hbrBackground = static_cast<::HBRUSH>(::GetStockObject(BLACK_BRUSH)),

        // No menu
        .lpszMenuName = nullptr,

        // The class name is the same as the Vulkan appinfo's engine name
        .lpszClassName = BTX_NAME,
    };

    // Try to use this class definition
    auto const result = ::RegisterClassEx(&wcex);
    if(!SUCCEEDED(result)) {
        auto const error = ::GetLastError();
        BTX_CRITICAL(
            "Could register window class. Error {}: '{}'",
            error,
            std::system_category().message(static_cast<int>(error))
        );
        return;
    }
}

// =============================================================================
void Win32TargetWindow::_create_window() {
    // If width and height aren't provided by Application, then just opt for
    // three-quarters of the available real estate
    if(_window_size.width == 0u || _window_size.height == 0u) {
        auto const w = static_cast<float>(_screen_size.width);
        auto const h = static_cast<float>(_screen_size.height);

        _window_size = {
            static_cast<uint32_t>(w * 0.75f),
            static_cast<uint32_t>(h * 0.75f)
        };
    }

    // Default to a centered window
    if(_window_pos.x == 0 || _window_pos.y == 0) {
        auto const half_width = static_cast<int32_t>(_window_size.width) / 2;
        auto const half_height = static_cast<int32_t>(_window_size.height) / 2;

        _window_pos = { _screen_center.x - half_width,
                        _screen_center.y - half_height };
    }

    // Create!
    _window_handle = ::CreateWindowExA(
        0u,             // No extended style
        BTX_NAME,       // Win32 class name
        _window_title,  // Win32 window title
        WS_POPUP,       // Popup window style means no decorations
        CW_USEDEFAULT,  // x location
        CW_USEDEFAULT,  // y location
        CW_USEDEFAULT,  // Window width
        CW_USEDEFAULT,  // Window height
        nullptr,        // Parent window handle
        nullptr,        // Menu handle
        nullptr,        // Instance handle
        this            // Pointer to lParam; retrieved later via WM_NCCREATE
    );

    if(_window_handle == nullptr) {
        auto const error = ::GetLastError();
        BTX_CRITICAL(
            "Failed to create win32 target window. Error {}: '{}'",
            error,
            std::system_category().message(static_cast<int>(error))
        );
        return;
    }

    BTX_INFO("Created win32 target window");
}

// =============================================================================
void Win32TargetWindow::_destroy_window() {
    BTX_TRACE("Destroying win32 target window.");
    ::SendMessageA(_window_handle, WM_CLOSE, 0, 0);
    Win32TargetWindow::message_loop();
}

// =============================================================================
void Win32TargetWindow::_register_raw_input() {
    // Below, we specify the keyboard and mouse as raw input devices. The
    // RIDEV_NOLEGACY flag tells Windows not to generate legacy messages, like
    // WM_KEYDOWN, while RIDEV_REMOVE unregisters the device from Windows' raw
    // input.

    std::array<::RAWINPUTDEVICE, 2> devices { };

    devices[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
    devices[0].usUsage     = HID_USAGE_GENERIC_KEYBOARD; // First, the keyboard
    devices[0].dwFlags     = RIDEV_NOLEGACY;
    devices[0].hwndTarget  = _window_handle;

    devices[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
    devices[1].usUsage     = HID_USAGE_GENERIC_MOUSE; // And then the mouse
    devices[1].dwFlags     = RIDEV_NOLEGACY;
    devices[1].hwndTarget  = _window_handle;

    // Attempt to register
    auto const result = ::RegisterRawInputDevices(
        devices.data(),
        static_cast<::UINT>(devices.size()),
        sizeof(::RAWINPUTDEVICE)
    );

    if(result == FALSE) {
        auto const error = ::GetLastError();
        BTX_CRITICAL("Failed to register raw win32 input with error: '{}'",
                     std::system_category().message(static_cast<int>(error)));
    }
    else {
        BTX_TRACE("Registered win32 raw input");
    }
}

// =============================================================================
void Win32TargetWindow::_deregister_raw_input() {
    // All of this is the same as in _register_raw_input()(), except RIDEV_NOLEGACY
    // is exchanged for RIDEV_REMOVE and provide nullptr instead of the window
    // handle

    std::array<::RAWINPUTDEVICE, 2> devices { };

    devices[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
    devices[0].usUsage     = HID_USAGE_GENERIC_KEYBOARD; // The keyboard
    devices[0].dwFlags     = RIDEV_REMOVE;
    devices[0].hwndTarget  = nullptr;

    devices[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
    devices[1].usUsage     = HID_USAGE_GENERIC_MOUSE; // And the mouse
    devices[1].dwFlags     = RIDEV_REMOVE;
    devices[1].hwndTarget  = nullptr;

    // Attempt to deregister
    auto const result = ::RegisterRawInputDevices(
        devices.data(),
        static_cast<::UINT>(devices.size()),
        sizeof(::RAWINPUTDEVICE)
    );

    if(result == FALSE) {
        auto const error = ::GetLastError();
        BTX_CRITICAL("Failed to deregister raw input with error: '{}'",
                     std::system_category().message(static_cast<int>(error)));
    }
    else {
        BTX_TRACE("Deregistered win32 raw input");
    }
}

// =============================================================================
void Win32TargetWindow::_restrict_cursor() {
    // Retrieve the window's drawable surface
	::RECT client_area;
	::GetClientRect(_window_handle, &client_area);

    // Since GetClientRect() doesn't tell us anything about where the window
    // is, we need to convert client_area to desktop coordinates
	::MapWindowPoints(
        _window_handle,
        HWND_DESKTOP,
        reinterpret_cast<::POINT *>(&client_area),
        2
    );

    // Now we can ensure the cursor stays within the window
	::ClipCursor(&client_area);

    // Run through all requests to show a cursor until ours is the last
    while(::ShowCursor(FALSE) >= 0) { }
}

// =============================================================================
void Win32TargetWindow::_release_cursor() {
    // Allow the cursor to travel outside the client space
    ::ClipCursor(nullptr);

    // Run through all requests to show a cursor until ours is the last
    while(::ShowCursor(TRUE) < 0) { }
}

// =============================================================================
void Win32TargetWindow::_size_and_place() {
    BTX_TRACE("Target window size: {}x{}, position: {}x{}",
              _window_size.width, _window_size.height,
              _window_pos.x, _window_pos.y);

    auto const result = ::SetWindowPos(
        _window_handle, nullptr,
        static_cast<int>(_window_pos.x),
        static_cast<int>(_window_pos.y),
        static_cast<int>(_window_size.width),
        static_cast<int>(_window_size.height),
        0
    );

    if(!SUCCEEDED(result)) {
        auto const error = ::GetLastError();
        BTX_CRITICAL("Failed to set win32 window position with error: '{}'",
                     std::system_category().message(static_cast<int>(error)));
    }
}

// =============================================================================
::LRESULT Win32TargetWindow::_static_wndproc(::HWND hWnd, ::UINT uMsg,
                                             ::WPARAM wParam, ::LPARAM lParam)
{
    static Win32TargetWindow *target = nullptr;

    if(uMsg == WM_NCCREATE) {
        // Recover the "this" pointer which was passed as a parameter
        // to CreateWindow
        auto const *lpcs = reinterpret_cast<::LPCREATESTRUCT>(lParam);
        target = static_cast<Win32TargetWindow *>(lpcs->lpCreateParams);

        // Put the value in a safe place for future use
        ::SetWindowLongPtrA(hWnd, GWLP_USERDATA,
                            reinterpret_cast<::LONG_PTR>(target));
    }
    else {
        // Recover the "this" pointer from where our WM_NCCREATE handler
        // stashed it.
        target = reinterpret_cast<Win32TargetWindow *>(
            ::GetWindowLongPtrA(hWnd, GWLP_USERDATA)
        );
    }

    if(target != nullptr) {
        // Now that we have recovered our "this" pointer, let the
        // member function finish the job.
        return target->_inst_wndproc(hWnd, uMsg, wParam, lParam);
    }

    // We don't know what our "this" pointer is, so just do the default
    // thing. Hopefully, we didn't need to customize the behavior yet.
    return ::DefWindowProcA(hWnd, uMsg, wParam, lParam);
}

// =============================================================================
::LRESULT Win32TargetWindow::_inst_wndproc(::HWND hWnd, ::UINT uMsg,
                                           ::WPARAM wParam, ::LPARAM lParam)
{
    // BTX_WARN("{}", _msg_map.translate(uMsg));

    switch(uMsg) {
        case WM_INPUT: {
            // First call to GetRawInputData is just to get the size of the
            // message so we can check it later
            ::UINT message_size;
            auto result = ::GetRawInputData(
                reinterpret_cast<::HRAWINPUT>(lParam),
                RID_INPUT,
                nullptr,
                &message_size,
                sizeof(::RAWINPUTHEADER)
            );

            if(result == ~0u) {
                BTX_CRITICAL("Failed to get win32 raw input message");
                break;
            }

            assert(message_size <= sizeof(::RAWINPUT));

            // Get the actual message
            result = ::GetRawInputData(
                reinterpret_cast<::HRAWINPUT>(lParam),
                RID_INPUT,
                _raw_msg,
                &message_size,
                sizeof(::RAWINPUTHEADER)
            );

            if(result != message_size) {
                BTX_CRITICAL(
                    "Win32 raw input expected {} bytes, got {} bytes instead",
                    message_size, result
                );
                break;
            }

            // Begin by casting to the type we're going to parse
            auto const *msg = reinterpret_cast<::RAWINPUT *>(_raw_msg);
            if(msg->header.dwType == RIM_TYPEKEYBOARD) {
                // Discard "fake keys" which are part of an escaped sequence,
                // otherwise process the input
                if(msg->data.keyboard.VKey != 255u) {
                    _parse_raw_keyboard(msg->data.keyboard);
                }
            }
            else if(msg->header.dwType == RIM_TYPEMOUSE) {
                _parse_raw_mouse(msg->data.mouse);
            }

            break;
        }

        case WM_ACTIVATE:
            if(wParam == WA_INACTIVE) {
                _deregister_raw_input();
                _release_cursor();
            }
            else {
                _register_raw_input();
                _restrict_cursor();
            }
            break;

        // This is the first message received in window close cascade. Note the
        // early return so there's no call to ::DefWindowProc()
        case WM_CLOSE:
            ::DestroyWindow(hWnd);
            ::UnregisterClassA(BTX_NAME, nullptr);
            return 0;

        // And this is the second message, but also the last one we have to
        // handle explicitly.
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;

        default: break;
    }

    // Be sure to let Windows handle what hasn't already been handled
    return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// =============================================================================
// Credit and big thanks to Stefan Reinalter for actually detailing the win32
// raw input stuff for the rest of us mortals:

// https://blog.molecular-matters.com/2011/09/05/properly-handling-keyboard-input/

void Win32TargetWindow::_parse_raw_keyboard(::RAWKEYBOARD const &raw) {
    // We might need to modify the virtual key code
    auto vkey = raw.VKey;

    // correct left-hand / right-hand SHIFT
    if(vkey == VK_SHIFT) {
        vkey = static_cast<::USHORT>(
            ::MapVirtualKeyA(raw.MakeCode, MAPVK_VSC_TO_VK_EX)
        );
    }

    // Windows keys
    else if(vkey == VK_LWIN) { vkey = BTX_KB_LEFT_SUPER; }
    else if(vkey == VK_RWIN) { vkey = BTX_KB_RIGHT_SUPER; }

    // And the remainder
    else {
        // e0 and e1 are escape sequences used for certain special
        // keys, such as PRINT and PAUSE/BREAK.
        // http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
        bool const isE0 = ((raw.Flags & RI_KEY_E0) != 0);

        if(isE0) {
            switch(vkey) {
                // right-hand CONTROL and ALT have their e0 bit set
                case VK_CONTROL: vkey = BTX_KB_RIGHT_CTRL; break;
                case VK_MENU   : vkey = BTX_KB_RIGHT_ALT;  break;

                // NUMPAD ENTER has its e0 bit set
                case VK_RETURN : vkey = BTX_KB_NP_ENTER;   break;

                default: break;
            }
        }
        else {
            switch(vkey) {
                case VK_CONTROL: vkey = BTX_KB_LEFT_CTRL; break;
                case VK_MENU   : vkey = BTX_KB_LEFT_ALT;  break;

                // the standard INSERT, DELETE, HOME, END, PRIOR and
                // NEXT keys will always have their e0 bit set, but the
                // corresponding keys on the NUMPAD will not.
                case VK_INSERT: vkey = BTX_KB_NP_0;       break;
                case VK_DELETE: vkey = BTX_KB_NP_DECIMAL; break;
                case VK_HOME  : vkey = BTX_KB_NP_7;       break;
                case VK_END   : vkey = BTX_KB_NP_1;       break;
                case VK_PRIOR : vkey = BTX_KB_NP_9;       break;
                case VK_NEXT  : vkey = BTX_KB_NP_3;       break;
                // the standard arrow keys will always have their e0 bit
                // set, but the corresponding keys on the NUMPAD will
                // not.
                case VK_LEFT  : vkey = BTX_KB_NP_4; break;
                case VK_RIGHT : vkey = BTX_KB_NP_6; break;
                case VK_UP    : vkey = BTX_KB_NP_8; break;
                case VK_DOWN  : vkey = BTX_KB_NP_2; break;

                // NUMPAD 5 doesn't have its e0 bit set
                case VK_CLEAR : vkey = BTX_KB_NP_5; break;

                default: break;
            }
        }
    }

    // Now that we've got everything sorted, do a lookup for the key code.
    auto const translated = _keymap.translate(vkey);

    // a key can either produce a "make" or "break" scancode. this is used to
    // differentiate between down-presses and releases
    // http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
    bool const is_release = ((raw.Flags & RI_KEY_BREAK) != 0);

    if(is_release) {
        EventBroker::emit<KeyReleaseEvent>(translated);
    }
    else {
        EventBroker::emit<KeyPressEvent>(translated);
    }
}

// =============================================================================
void Win32TargetWindow::_parse_raw_mouse(::RAWMOUSE const &raw) {
    auto const button_data = raw.usButtonData;
    auto const button_flags = raw.usButtonFlags;

    // Left mouse button
    if((button_flags & RI_MOUSE_LEFT_BUTTON_DOWN) != 0u) {
        EventBroker::emit<MouseButtonPressEvent>(BTX_MB_LEFT);
    }
    else if((button_flags & RI_MOUSE_LEFT_BUTTON_UP) != 0u) {
        EventBroker::emit<MouseButtonReleaseEvent>(BTX_MB_LEFT);
    }

    // Right mouse button
    if((button_flags & RI_MOUSE_RIGHT_BUTTON_DOWN) != 0u) {
        EventBroker::emit<MouseButtonPressEvent>(BTX_MB_RIGHT);
    }
    else if((button_flags & RI_MOUSE_RIGHT_BUTTON_UP) != 0u) {
        EventBroker::emit<MouseButtonReleaseEvent>(BTX_MB_RIGHT);
    }

    // Middle mouse button
    if((button_flags & RI_MOUSE_MIDDLE_BUTTON_DOWN) != 0u) {
        EventBroker::emit<MouseButtonPressEvent>(BTX_MB_MIDDLE);
    }
    else if((button_flags & RI_MOUSE_MIDDLE_BUTTON_UP) != 0u) {
        EventBroker::emit<MouseButtonReleaseEvent>(BTX_MB_MIDDLE);
    }

    // Mouse movement
    if(raw.lLastX != 0 || raw.lLastY != 0) {
        EventBroker::emit<MouseMoveEvent>(raw.lLastX, raw.lLastY);
    }

    // Vertical mouse scroll
    if((button_flags & RI_MOUSE_WHEEL) != 0u) {
        EventBroker::emit<MouseScrollEvent>(
            static_cast<int32_t>(button_data),
            0
        );
    }

    // Horizontal mouse scroll
    if((button_flags & RI_MOUSE_HWHEEL) != 0u) {
        EventBroker::emit<MouseScrollEvent>(
            0,
            static_cast<int32_t>(button_data)
        );
    }
}

} // namespace btx

#endif // BTX_WINDOWS