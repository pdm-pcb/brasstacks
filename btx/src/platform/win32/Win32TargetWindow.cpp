#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/win32/Win32TargetWindow.hpp"

#include "brasstacks/events/EventBroker.hpp"
#include "brasstacks/events/window_events.hpp"
#include "brasstacks/events/keyboard_events.hpp"
#include "brasstacks/events/mouse_events.hpp"
#include "brasstacks/platform/win32/Win32ToBTXKeys.hpp"
#include "brasstacks/system/GraphicsAPI.hpp"

namespace btx {

::LPCSTR Win32TargetWindow::_window_title = nullptr;
::HWND   Win32TargetWindow::_window       = nullptr;
::HDC    Win32TargetWindow::_device       = nullptr;

std::unique_ptr<char> Win32TargetWindow::_raw_msg;

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
    _raw_msg = std::make_unique<char>(sizeof(::RAWINPUTHEADER));
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
void Win32TargetWindow::create_window(Dimensions const &dimensions,
                                       Position const &position)
{
    if(_window != nullptr) {
        BTX_CRITICAL("Only one target window at a time is allowed.");
        return;
    }

    // If width and height aren't provided by Application, then just opt for
    // two-thirds of the available real estate
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
        0u,                     // extended style
        BTX_NAME,               // win32 class name
        _window_title,          // win32 window title
        WS_POPUP | WS_VISIBLE,  // No decorations, visible by default
        CW_USEDEFAULT,          // x location
        CW_USEDEFAULT,          // y location
        static_cast<int>(RenderConfig::window_width),   // width
        static_cast<int>(RenderConfig::window_height),  // height
        nullptr,    // parent window handle
        nullptr,    // menu handle
        nullptr,    // instance handle
        nullptr     // pointer to lParam; retrieved via WM_CREATE
    );

    if(_window == nullptr) {
        auto const error = ::GetLastError();
        BTX_CRITICAL(
            "Failed to create win32 window. Error {}: '{}'",
            error,
            std::system_category().message(static_cast<int>(error))
        );
        return;
    }

    _register_input();
    _size_and_place();

    BTX_TRACE(
        "Created Win32 target window: {}x{} @ {:0.4f}",
        RenderConfig::window_width,
        RenderConfig::window_height,
        RenderConfig::window_aspect
    );
}

// =============================================================================
void Win32TargetWindow::create_surface() {
    // The details Vulkan cares about
    vk::Win32SurfaceCreateInfoKHR const surface_info {
        .hinstance = nullptr,
        .hwnd = _window,
    };

    // Create, check, assign
    auto result = GraphicsAPI::native().createWin32SurfaceKHR(surface_info);
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
    GraphicsAPI::native().destroy(_surface);
}

// =============================================================================
::LRESULT Win32TargetWindow::_wndproc(::HWND window, ::UINT message,
                                      ::WPARAM wparam, ::LPARAM lparam)
{
    switch(message) {
        case WM_INPUT: BTX_WARN("WM_INPUT"); {
            // First call to GetRawInputData is just to get the size of the
            // message so we can check it later
            ::UINT message_size;
            auto result = ::GetRawInputData(
                reinterpret_cast<::HRAWINPUT>(lparam),
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
                reinterpret_cast<::HRAWINPUT>(lparam),
                RID_INPUT,
                _raw_msg.get(),
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
            auto const *msg = reinterpret_cast<::RAWINPUT *>(_raw_msg.get());
            if(msg->header.dwType == RIM_TYPEKEYBOARD) {
                // a key can either produce a "make" or "break" scancode.
                // this is used to differentiate between down-presses and
                // releases
                // http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
                bool const is_release =
                    ((msg->data.keyboard.Flags & RI_KEY_BREAK) != 0);

                // Escape quits the whole program
                if(msg->data.keyboard.VKey == VK_ESCAPE && !is_release) {
                    ::SendMessage(window, WM_CLOSE, wparam, lparam);
                }
                // Discard "fake keys" which are part of an escaped sequence
                else if(msg->data.keyboard.VKey == 255u) { }
                // Otherwise, parse away
                else {
                    _parse_raw_keyboard(msg->data.keyboard, is_release);
                }
            }
            else if(msg->header.dwType == RIM_TYPEMOUSE) {
                _parse_raw_mouse(msg->data.mouse);
            }

            break;
        }

        case WM_CREATE: BTX_WARN("WM_CREATE");
            _restrict_cursor();
            break;

        case WM_ACTIVATE:
            if(wparam == WA_ACTIVE || wparam == WA_CLICKACTIVE) {
                BTX_WARN("WM_ACTIVATE WA_ACTIVE");
                _restrict_cursor();
            }
            else if(wparam == WA_INACTIVE) {
                BTX_WARN("WM_ACTIVATE WA_INACTIVE");
                _release_cursor();
            }
            break;

        case WM_CLOSE: BTX_WARN("WM_CLOSE");
            ::DestroyWindow(_window);
            ::UnregisterClass(BTX_NAME, nullptr);
            return 0;

        case WM_DESTROY: BTX_WARN("WM_DESTROY");
            ::PostQuitMessage(0);
            _release_cursor();
            EventBroker::emit<WindowCloseEvent>();
            return 0;

        case WM_SETFOCUS        : BTX_WARN("WM_SETFOCUS");          break;
        case WM_KILLFOCUS       : BTX_WARN("WM_KILLFOCUS");         break;
        case WM_ENABLE          : BTX_WARN("WM_ENABLE");            break;
        case WM_SETREDRAW       : BTX_WARN("WM_SETREDRAW");         break;
        case WM_SETTEXT         : BTX_WARN("WM_SETTEXT");           break;
        case WM_GETTEXT         : BTX_WARN("WM_GETTEXT");           break;
        case WM_GETTEXTLENGTH   : BTX_WARN("WM_GETTEXTLENGTH");     break;
        case WM_PAINT           : BTX_WARN("WM_PAINT");             break;
        case WM_QUERYENDSESSION : BTX_WARN("WM_QUERYENDSESSION");   break;
        case WM_QUERYOPEN       : BTX_WARN("WM_QUERYOPEN");         break;
        case WM_ENDSESSION      : BTX_WARN("WM_ENDSESSION");        break;
        case WM_MOVE            : BTX_WARN("WM_MOVE");              break;
        case WM_SIZE            : BTX_WARN("WM_SIZE");              break;
        case WM_NOTIFY: BTX_WARN("WM_NOTIFY"); break;
        case WM_INPUTLANGCHANGEREQUEST: BTX_WARN("WM_INPUTLANGCHANGEREQUEST"); break;
        case WM_INPUTLANGCHANGE: BTX_WARN("WM_INPUTLANGCHANGE"); break;
        case WM_TCARD: BTX_WARN("WM_TCARD"); break;
        case WM_HELP: BTX_WARN("WM_HELP"); break;
        case WM_USERCHANGED: BTX_WARN("WM_USERCHANGED"); break;
        case WM_NOTIFYFORMAT: BTX_WARN("WM_NOTIFYFORMAT"); break;
        case WM_CONTEXTMENU: BTX_WARN("WM_CONTEXTMENU"); break;
        case WM_STYLECHANGING: BTX_WARN("WM_STYLECHANGING"); break;
        case WM_STYLECHANGED: BTX_WARN("WM_STYLECHANGED"); break;
        case WM_DISPLAYCHANGE: BTX_WARN("WM_DISPLAYCHANGE"); break;
        case WM_GETICON: BTX_WARN("WM_GETICON"); break;
        case WM_SETICON: BTX_WARN("WM_SETICON"); break;
        case WM_PAINTICON: BTX_WARN("WM_PAINTICON"); break;
        case WM_ICONERASEBKGND: BTX_WARN("WM_ICONERASEBKGND"); break;
        case WM_NEXTDLGCTL: BTX_WARN("WM_NEXTDLGCTL"); break;
        case WM_SPOOLERSTATUS: BTX_WARN("WM_SPOOLERSTATUS"); break;
        case WM_DRAWITEM: BTX_WARN("WM_DRAWITEM"); break;
        case WM_MEASUREITEM: BTX_WARN("WM_MEASUREITEM"); break;
        case WM_DELETEITEM: BTX_WARN("WM_DELETEITEM"); break;
        case WM_VKEYTOITEM: BTX_WARN("WM_VKEYTOITEM"); break;
        case WM_CHARTOITEM: BTX_WARN("WM_CHARTOITEM"); break;
        case WM_SETFONT: BTX_WARN("WM_SETFONT"); break;
        case WM_GETFONT: BTX_WARN("WM_GETFONT"); break;
        case WM_SETHOTKEY: BTX_WARN("WM_SETHOTKEY"); break;
        case WM_GETHOTKEY: BTX_WARN("WM_GETHOTKEY"); break;
        case WM_QUERYDRAGICON: BTX_WARN("WM_QUERYDRAGICON"); break;
        case WM_COMPAREITEM: BTX_WARN("WM_COMPAREITEM"); break;
        case WM_GETOBJECT: BTX_WARN("WM_GETOBJECT"); break;
        case WM_COMPACTING: BTX_WARN("WM_COMPACTING"); break;
        case WM_COMMNOTIFY: BTX_WARN("WM_COMMNOTIFY"); break;
        case WM_WINDOWPOSCHANGING: BTX_WARN("WM_WINDOWPOSCHANGING"); break;
        case WM_WINDOWPOSCHANGED: BTX_WARN("WM_WINDOWPOSCHANGED"); break;
        case WM_NCCREATE: BTX_WARN("WM_NCCREATE"); break;
        case WM_NCDESTROY: BTX_WARN("WM_NCDESTROY"); break;
        case WM_NCCALCSIZE: BTX_WARN("WM_NCCALCSIZE"); break;
        case WM_NCHITTEST: BTX_WARN("WM_NCHITTEST"); break;
        case WM_NCPAINT: BTX_WARN("WM_NCPAINT"); break;
        case WM_NCACTIVATE: BTX_WARN("WM_NCACTIVATE"); break;
        case WM_GETDLGCODE: BTX_WARN("WM_GETDLGCODE"); break;
        case WM_NCMOUSEMOVE: BTX_WARN("WM_NCMOUSEMOVE"); break;
        case WM_NCLBUTTONDOWN: BTX_WARN("WM_NCLBUTTONDOWN"); break;
        case WM_NCLBUTTONUP: BTX_WARN("WM_NCLBUTTONUP"); break;
        case WM_NCLBUTTONDBLCLK: BTX_WARN("WM_NCLBUTTONDBLCLK"); break;
        case WM_NCRBUTTONDOWN: BTX_WARN("WM_NCRBUTTONDOWN"); break;
        case WM_NCRBUTTONUP: BTX_WARN("WM_NCRBUTTONUP"); break;
        case WM_NCRBUTTONDBLCLK: BTX_WARN("WM_NCRBUTTONDBLCLK"); break;
        case WM_NCMBUTTONDOWN: BTX_WARN("WM_NCMBUTTONDOWN"); break;
        case WM_NCMBUTTONUP: BTX_WARN("WM_NCMBUTTONUP"); break;
        case WM_NCMBUTTONDBLCLK: BTX_WARN("WM_NCMBUTTONDBLCLK"); break;
        case WM_IME_SETCONTEXT: BTX_WARN("WM_IME_SETCONTEXT"); break;
        case WM_IME_NOTIFY: BTX_WARN("WM_IME_NOTIFY"); break;
        case WM_IME_CONTROL: BTX_WARN("WM_IME_CONTROL"); break;
        case WM_IME_COMPOSITIONFULL: BTX_WARN("WM_IME_COMPOSITIONFULL"); break;
        case WM_IME_SELECT: BTX_WARN("WM_IME_SELECT"); break;
        case WM_IME_CHAR: BTX_WARN("WM_IME_CHAR"); break;
        case WM_ERASEBKGND: BTX_WARN("WM_ERASEBKGND"); break;
        case WM_SYSCOLORCHANGE: BTX_WARN("WM_SYSCOLORCHANGE"); break;
        case WM_SHOWWINDOW: BTX_WARN("WM_SHOWWINDOW"); break;
        case WM_WININICHANGE: BTX_WARN("WM_WININICHANGE"); break;

        default: BTX_WARN("{:#x}", message); break;
    }

    // Be sure to let Windows handle what hasn't already been handled
    return ::DefWindowProc(window, message, wparam, lparam);
}

// =============================================================================
// Credit and big thanks to Stefan Reinalter for actually detailing the win32
// raw input stuff for the rest of us mortals:

// https://blog.molecular-matters.com/2011/09/05/properly-handling-keyboard-input/

void Win32TargetWindow::_parse_raw_keyboard(::RAWKEYBOARD const &raw,
                                            bool const is_release)
{
    // We might need to modify the virtual key code
    auto vkey = raw.VKey;

    // correct left-hand / right-hand SHIFT
    if(vkey == VK_SHIFT) {
        vkey = static_cast<::USHORT>(
            MapVirtualKey(raw.MakeCode, MAPVK_VSC_TO_VK_EX)
        );
    }

    // Windows keys
    else if(vkey == VK_LWIN) { vkey = KB_LEFT_SUPER; }
    else if(vkey == VK_RWIN) { vkey = KB_RIGHT_SUPER; }

    // And the remainder
    else {
        // e0 and e1 are escape sequences used for certain special
        // keys, such as PRINT and PAUSE/BREAK.
        // http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
        bool const isE0 = ((raw.Flags & RI_KEY_E0) != 0);

        if(isE0) {
            switch(vkey) {
                // right-hand CONTROL and ALT have their e0 bit set
                case VK_CONTROL: vkey = KB_RIGHT_CTRL; break;
                case VK_MENU   : vkey = KB_RIGHT_ALT;  break;

                // NUMPAD ENTER has its e0 bit set
                case VK_RETURN : vkey = KB_NP_ENTER;   break;

                default: break;
            }
        }
        else {
            switch(vkey) {
                case VK_CONTROL: vkey = KB_LEFT_CTRL; break;
                case VK_MENU   : vkey = KB_LEFT_ALT;  break;

                // the standard INSERT, DELETE, HOME, END, PRIOR and
                // NEXT keys will always have their e0 bit set, but the
                // corresponding keys on the NUMPAD will not.
                case VK_INSERT: vkey = KB_NP_0;       break;
                case VK_DELETE: vkey = KB_NP_DECIMAL; break;
                case VK_HOME  : vkey = KB_NP_7;       break;
                case VK_END   : vkey = KB_NP_1;       break;
                case VK_PRIOR : vkey = KB_NP_9;       break;
                case VK_NEXT  : vkey = KB_NP_3;       break;
                // the standard arrow keys will always have their e0 bit
                // set, but the corresponding keys on the NUMPAD will
                // not.
                case VK_LEFT  : vkey = KB_NP_4; break;
                case VK_RIGHT : vkey = KB_NP_6; break;
                case VK_UP    : vkey = KB_NP_8; break;
                case VK_DOWN  : vkey = KB_NP_2; break;

                // NUMPAD 5 doesn't have its e0 bit set
                case VK_CLEAR : vkey = KB_NP_5; break;

                default: break;
            }
        }
    }

    if(is_release) {
        EventBroker::emit<KeyReleaseEvent>(Win32ToBTXKeys::map.at(vkey));
    }
    else {
        EventBroker::emit<KeyPressEvent>(Win32ToBTXKeys::map.at(vkey));
    }
}

// =============================================================================
void Win32TargetWindow::_parse_raw_mouse(::RAWMOUSE const &raw) {
    auto const button_data = raw.usButtonData;
    auto const button_flags = raw.usButtonFlags;

    // Testing for the left mouse button
    if((button_flags & RI_MOUSE_LEFT_BUTTON_DOWN) != 0u) {
        EventBroker::emit<MouseButtonPressEvent>(MB_LEFT);
    }
    else if((button_flags & RI_MOUSE_LEFT_BUTTON_UP) != 0u) {
        EventBroker::emit<MouseButtonReleaseEvent>(MB_LEFT);
    }

    // Testing for the right mouse button
    if((button_flags & RI_MOUSE_RIGHT_BUTTON_DOWN) != 0u) {
        EventBroker::emit<MouseButtonPressEvent>(MB_RIGHT);
    }
    else if((button_flags & RI_MOUSE_RIGHT_BUTTON_UP) != 0u) {
        EventBroker::emit<MouseButtonReleaseEvent>(MB_RIGHT);
    }

    // Testing for the middle mouse button
    if((button_flags & RI_MOUSE_MIDDLE_BUTTON_DOWN) != 0u) {
        EventBroker::emit<MouseButtonPressEvent>(MB_MIDDLE);
    }
    else if((button_flags & RI_MOUSE_MIDDLE_BUTTON_UP) != 0u) {
        EventBroker::emit<MouseButtonReleaseEvent>(MB_MIDDLE);
    }

    // Testing if the mouse moved
    if(raw.lLastX != 0 || raw.lLastY != 0) {
        EventBroker::emit<MouseMoveEvent>(raw.lLastX, raw.lLastY);
    }

    // Testing for a mouse wheel vertical scroll
    if((button_flags & RI_MOUSE_WHEEL) != 0u) {
        EventBroker::emit<MouseScrollEvent>(
            static_cast<int32_t>(button_data),
            0
        );
    }

    // Testing for a mouse wheel horizontal scroll
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
    while(::ShowCursor(FALSE) >= 0) {

    }
}

// =============================================================================
void Win32TargetWindow::_release_cursor() {
    // Allow the cursor to travel outside the client space
    ::ClipCursor(nullptr);

    // Queue requests until there are some
    while(::ShowCursor(TRUE) < 0) {

    }
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