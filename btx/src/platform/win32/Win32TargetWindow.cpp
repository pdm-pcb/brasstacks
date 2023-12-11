#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/win32/Win32TargetWindow.hpp"

#include "brasstacks/events/EventBroker.hpp"
#include "brasstacks/events/keyboard_events.hpp"
#include "brasstacks/events/mouse_events.hpp"
#include "brasstacks/platform/win32/Win32ToBTXKeys.hpp"
#include "brasstacks/platform/vulkan/VkInstance.hpp"

namespace btx {

::LPCSTR Win32TargetWindow::_window_title = nullptr;
::HWND   Win32TargetWindow::_window       = nullptr;
::HDC    Win32TargetWindow::_device       = nullptr;

char *Win32TargetWindow::_raw_msg = nullptr;

Win32TargetWindow::Position Win32TargetWindow::_screen_center = { 0, 0 };

vk::SurfaceKHR Win32TargetWindow::_surface = { };

// =============================================================================
void Win32TargetWindow::init(std::string_view const app_name) {
    // Use the user-provided name as the window title
    _window_title = app_name.data();

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
    RenderConfig::screen_width  =
        static_cast<uint32_t>(::GetSystemMetrics(SM_CXSCREEN));
    RenderConfig::screen_height =
        static_cast<uint32_t>(::GetSystemMetrics(SM_CYSCREEN));

    // Hold on to the screen's center for later
    _screen_center.x = static_cast<int32_t>(RenderConfig::screen_width) / 2;
    _screen_center.y = static_cast<int32_t>(RenderConfig::screen_height) / 2;

    // Now it's time to register the window class
    ::WNDCLASSEX const wcex {
        .cbSize = sizeof(::WNDCLASSEX),

        // These flags instruct Windows to redraw the entire window surface if
        // the size changes; by default it'll just refresh the new real estate
        .style = CS_HREDRAW | CS_VREDRAW,

        // Specify the callback for Windows messages
        .lpfnWndProc = _wndproc,

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

    // Allocate a struct for the raw device input messages
    _raw_msg = new char[sizeof(::RAWINPUTHEADER)];
}

// =============================================================================
void Win32TargetWindow::shutdown() {
    // Clean up our one allocation
    delete[] _raw_msg;
}

// =============================================================================
void Win32TargetWindow::create_window(Dimensions const &dimensions,
                                      Position const &position)
{
    if(_window != nullptr) {
        BTX_CRITICAL("Only one target window at a time is allowed.");
        return;
    }

    // If width and height aren't provided by Application, then just opt for
    // three-quarters of the available real estate
    if(dimensions.width == 0u || dimensions.height == 0u) {
        auto width_fraction  = static_cast<float>(RenderConfig::screen_width);
        auto height_fraction = static_cast<float>(RenderConfig::screen_height);
        width_fraction  *= 0.75f;
        height_fraction *= 0.75f;

        RenderConfig::window_width  = static_cast<uint32_t>(width_fraction);
        RenderConfig::window_height = static_cast<uint32_t>(height_fraction);
    }
    else {
        RenderConfig::window_width = dimensions.width;
        RenderConfig::window_height = dimensions.height;
    }

    // Determine the window's eventual position on screen
    auto const half_width =
        static_cast<int32_t>(RenderConfig::window_width)  / 2;
    auto const half_height =
        static_cast<int32_t>(RenderConfig::window_height) / 2;

    if(position.x == 0 || position.y == 0) {
        RenderConfig::window_pos_x = _screen_center.x - half_width;
        RenderConfig::window_pos_y = _screen_center.y - half_height;
    }
    else {
        RenderConfig::window_pos_x = position.x;
        RenderConfig::window_pos_y = position.y;
    }

    // Create!
    _window = ::CreateWindowEx(
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
        nullptr         // Pointer to lParam; retrieved later via WM_CREATE
    );

    if(_window == nullptr) {
        auto const error = ::GetLastError();
        BTX_CRITICAL(
            "Failed to create win32 target window. Error {}: '{}'",
            error,
            std::system_category().message(static_cast<int>(error))
        );
        return;
    }

    BTX_TRACE(
        "Created win32 target window: {}x{}",
        RenderConfig::window_width,
        RenderConfig::window_height
    );

    _register_input();
    _size_and_place();
}

// =============================================================================
void Win32TargetWindow::show_window() {
    ::ShowWindow(_window, SW_SHOWNORMAL);
}

// =============================================================================
void Win32TargetWindow::destroy_window() {
    BTX_TRACE("Destroying win32 target window.");
    ::SendMessage(_window, WM_CLOSE, 0, 0);
    message_loop();
}

// =============================================================================
void Win32TargetWindow::create_surface() {
    // The details Vulkan cares about
    vk::Win32SurfaceCreateInfoKHR const surface_info {
        .hinstance = nullptr,
        .hwnd = _window,
    };

    // Create, check, assign
    auto result = VkInstance::native().createWin32SurfaceKHR(surface_info);
    if(result.result != vk::Result::eSuccess) {
        BTX_CRITICAL(
            "Unable to create Win32 KHR surface: '{}'",
            to_string(result.result)
        );
    }
    _surface = result.value;

    BTX_TRACE(
        "Created Vulkan surface {:#x}",
        reinterpret_cast<uint64_t>(::VkSurfaceKHR(_surface))
    );
}

// =============================================================================
void Win32TargetWindow::destroy_surface() {
    BTX_TRACE(
        "Destroying Vulkan surface {:#x}",
        reinterpret_cast<uint64_t>(::VkSurfaceKHR(_surface))
    );
    VkInstance::native().destroy(_surface);
}

// =============================================================================
void Win32TargetWindow::message_loop() {
    ::MSG message;
    std::memset(&message, 0, sizeof(::MSG));

    // Run through available messages from the OS
    while(::PeekMessage(&message, _window, 0u, 0u, PM_REMOVE) != 0) {
        ::DispatchMessage(&message);
    }
}

// =============================================================================
::LRESULT Win32TargetWindow::_wndproc(::HWND hWnd, ::UINT uMsg,
                                      ::WPARAM wParam, ::LPARAM lParam)
{
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

        case WM_CREATE:
            _restrict_cursor();
            break;

        case WM_ACTIVATE:
            if(wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE) {
                _restrict_cursor();
            }
            else if(wParam == WA_INACTIVE) {
                _release_cursor();
            }
            break;

        // This is the first message received in window close cascade. Note the
        // early return so there's no call to ::DefWindowProc()
        case WM_CLOSE:
            ::DestroyWindow(hWnd);
            ::UnregisterClass(BTX_NAME, nullptr);
            return 0;

        // And this is the second message, but also the last one we have to
        // handle explicitly.
        case WM_DESTROY:
            ::PostQuitMessage(0);
            _release_cursor();
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
            MapVirtualKey(raw.MakeCode, MAPVK_VSC_TO_VK_EX)
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

    // Now that we've got everything sorted, do a lookup for the key code. If
    // it's not in the map, return early.
    auto const translated = Win32ToBTXKeys::map.find(vkey);
    if(translated == Win32ToBTXKeys::map.end()) {
        BTX_WARN("Unknown win32 vkey: {}", vkey);
        return;
    }

    // a key can either produce a "make" or "break" scancode. this is used to
    // differentiate between down-presses and releases
    // http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
    bool const is_release = ((raw.Flags & RI_KEY_BREAK) != 0);

    if(is_release) {
        EventBroker::emit<KeyReleaseEvent>(translated->second);
    }
    else {
        EventBroker::emit<KeyPressEvent>(translated->second);
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

// =============================================================================
void Win32TargetWindow::_register_input() {
    if(_raw_msg == nullptr) {
        BTX_CRITICAL(
            "Win32TargetWindow::_raw_msg cannot be null. "
            "Did Win32TargetWindow::init() get called?"
        );
        return;
    }

    ::RAWINPUTDEVICE devices[2];

    devices[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
    devices[0].usUsage     = HID_USAGE_GENERIC_KEYBOARD;
    // The RIDEV_NOLEGACY flag tells the raw device not to generate legacy
    // messages, like WM_KEYDOWN
    devices[0].dwFlags     = RIDEV_NOLEGACY;
    devices[0].hwndTarget  = _window;

    devices[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
    devices[1].usUsage     = HID_USAGE_GENERIC_MOUSE;
    devices[1].dwFlags     = RIDEV_NOLEGACY; // Same as above, but for mouse
    devices[1].hwndTarget  = _window;

    auto const result =
        ::RegisterRawInputDevices(devices, 2, sizeof(::RAWINPUTDEVICE));

    if(result == FALSE) {
        auto const error = ::GetLastError();
        BTX_CRITICAL(
            "Failed to register for raw win32 input with error: '{}'",
            std::system_category().message(static_cast<int>(error))
        );
        return;
    }

    BTX_TRACE("Registered for raw win32 input");
}

// =============================================================================
void Win32TargetWindow::_restrict_cursor() {
    ::SetCursorPos(_screen_center.x, _screen_center.y);

	::RECT client_area;
	::GetClientRect(_window, &client_area);
	::MapWindowPoints(
        _window,
        nullptr, // Convert window-relative coordinates to desktop coordinates
        reinterpret_cast<::POINT *>(&client_area),
        2
    );
    // Restrict the cursor to moving within the client area
	::ClipCursor(&client_area);

    // Run through all requests to show a cursor until there are none
    while(::ShowCursor(FALSE) >= 0) { }
}

// =============================================================================
void Win32TargetWindow::_release_cursor() {
    // Allow the cursor to travel outside the client space
    ::ClipCursor(nullptr);

    // Queue requests until there are some
    while(::ShowCursor(TRUE) < 0) { }
}

// =============================================================================
void Win32TargetWindow::_size_and_place() {
    BTX_TRACE(
        "Window size: {}x{}, position: {}x{}",
        RenderConfig::window_width, RenderConfig::window_height,
        RenderConfig::window_pos_x, RenderConfig::window_pos_y
    );

    // Update the window aspect ratio
    RenderConfig::window_aspect =
        static_cast<float>(RenderConfig::window_width) /
        static_cast<float>(RenderConfig::window_height);

    auto const result = ::SetWindowPos(
        _window, nullptr,
        static_cast<int>(RenderConfig::window_pos_x),
        static_cast<int>(RenderConfig::window_pos_y),
        static_cast<int>(RenderConfig::window_width),
        static_cast<int>(RenderConfig::window_height),
        0
    );

    if(result == FALSE) {
        auto const error = ::GetLastError();
        BTX_CRITICAL(
            "Failed to set win32 window position with error: '{}'",
            std::system_category().message(static_cast<int>(error))
        );
        return;
    }
}

} // namespace btx