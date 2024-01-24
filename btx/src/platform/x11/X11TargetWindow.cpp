#ifdef BTX_LINUX

#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/x11/X11TargetWindow.hpp"

#include "brasstacks/events/EventBus.hpp"
#include "brasstacks/events/window_events.hpp"
#include "brasstacks/events/keyboard_events.hpp"
#include "brasstacks/events/mouse_events.hpp"
#include "brasstacks/platform/vulkan/vkInstance.hpp"

namespace btx {

X11MsgToStr const X11TargetWindow::_msg_map;
X11ToBTXKeys const X11TargetWindow::_keymap;

// =============================================================================
X11TargetWindow::X11TargetWindow(std::string_view const app_name) :
    _display               { nullptr },
    _root                  { 0u },
    _window                { 0u },
    _visual_info           { },
    _attributes            { },
    _window_title          { app_name },
    _screen_size           { 0u, 0u },
    _screen_center         { 0, 0 },
    _window_size           { 0u, 0u },
    _window_position       { 0, 0 },
    _size_mutex            { },
    _position_mutex        { },
    _minimized             { false },
    _run_flag              { },
    _toggle_cursor_capture { false },
    _cursor_captured       { false }
{
    _display = ::XOpenDisplay(nullptr);

    if(_display == nullptr) {
        BTX_CRITICAL("Could not open default X11 display");
        return;
    }

    _root = ::XDefaultRootWindow(_display);

    if(_root == 0u) {
        BTX_CRITICAL("Could not get default X11 root window");
        ::XCloseDisplay(_display);
        return;
    }

    auto * const screen = ::XDefaultScreenOfDisplay(_display);

    if(screen == nullptr) {
        BTX_CRITICAL("Could not open default X11 screen");
        ::XCloseDisplay(_display);
        return;
    }

    // Testing for the primary display's resolution this way feels brittle,
    // but it does work for the average use case
    _screen_size = {
        .width = static_cast<uint32_t>(screen->width),
        .height = static_cast<uint32_t>(screen->height)
    };

    // Hold on to the screen's center for later
    _screen_center = {
        .x = static_cast<int32_t>(_screen_size.width / 2u),
        .y = static_cast<int32_t>(_screen_size.height / 2u)
    };

    auto const visinfo_match = ::XMatchVisualInfo(
        _display,
        ::XScreenNumberOfScreen(screen),
        RenderConfig::display_bit_depth,
        TrueColor,
        &_visual_info
    );

    if(visinfo_match == 0) {
        BTX_CRITICAL("Unable to match X11 visual info with bit depth {}",
                     RenderConfig::display_bit_depth);
        ::XCloseDisplay(_display);
        return;
    }

    _attributes.background_pixel = 0u;
    _attributes.colormap = ::XCreateColormap(_display,
                                             _root,
                                             _visual_info.visual,
                                             AllocNone);
    _attributes.event_mask = StructureNotifyMask;
}

// =============================================================================
X11TargetWindow::~X11TargetWindow() {
    ::XCloseDisplay(_display);
}

// =============================================================================
void X11TargetWindow::start() {
    BTX_TRACE("Starting target window...");
    _run_flag.test_and_set();
    _run_flag.notify_one();
}

// =============================================================================
void X11TargetWindow::stop() {
    BTX_TRACE("Stopping target window...");
    _run_flag.clear();
}

// =============================================================================
void X11TargetWindow::run() {
    _create_window();
    _size_and_place();

    BTX_TRACE("Target window ready to run...");
    _run_flag.wait(false);
    BTX_TRACE("Target window running!");

    ::XMapWindow(_display, _window);
    ::XFlush(_display);

    while(_run_flag.test()) {
        if(_toggle_cursor_capture.load()) {
            if(_cursor_captured) {
                BTX_TRACE("Releasing cursor");
                _cursor_captured = false;
                // _deregister_raw_input();
                // _release_cursor();
            }
            else {
                BTX_TRACE("Capturing cursor");
                _cursor_captured = true;
                // _restrict_cursor();
                // _register_raw_input();
            }

            _toggle_cursor_capture.store(false);
        }

        _message_loop();

        std::this_thread::yield();
    }

    // ::ShowWindow(_window, SW_HIDE);
    _destroy_window();
}

// =============================================================================
void X11TargetWindow::toggle_cursor_capture() {
    _toggle_cursor_capture.store(true);
}

// =============================================================================
void X11TargetWindow::_create_window() {
    // If width and height aren't preconfigured, then just opt for most of the
    // available real estate
    if(_window_size.width == 0u || _window_size.height == 0u) {
        auto const w = static_cast<float>(_screen_size.width);
        auto const h = static_cast<float>(_screen_size.height);

        std::unique_lock<std::mutex> write_lock(_size_mutex);
        _window_size = { .width = static_cast<uint32_t>(w * 0.5f),
                         .height = static_cast<uint32_t>(h * 0.5f) };
    }

    // Default to a centered window
    if(_window_position.x == 0 || _window_position.y == 0) {
        auto const half_width = static_cast<int32_t>(_window_size.width) / 2;
        auto const half_height = static_cast<int32_t>(_window_size.height) / 2;

        std::unique_lock<std::mutex> write_lock(_position_mutex);
        _window_position = { .x = _screen_center.x - half_width,
                             .y = _screen_center.y - half_height };
    }

    _window = ::XCreateWindow(_display, _root,
                              _window_position.x,
                              _window_position.y,
                              _window_size.width,
                              _window_size.height,
                              0, // border width
                              RenderConfig::display_bit_depth,
                              InputOutput, // window class
                              _visual_info.visual,
                              CWBackPixel | CWColormap | CWEventMask,
                              &_attributes);

    // auto scale = static_cast<float>(::GetDpiForWindow(_window));
    // scale /= static_cast<float>(USER_DEFAULT_SCREEN_DPI);
    // this->_set_dpi_scale(scale);

    if(_window == 0u) {
        BTX_CRITICAL("Failed to create x11 target window");
        ::XCloseDisplay(_display);
        return;
    }

    if(::XStoreName(_display, _window, _window_title.c_str()) == 0) {
        BTX_ERROR("Unable to set x11 target window title");
    }

    BTX_INFO("Created x11 target window");
}

// =============================================================================
void X11TargetWindow::_destroy_window() {
    BTX_TRACE("Destroying x11 target window.");
    ::XDestroyWindow(_display, _window);
}

// // =============================================================================
// void X11TargetWindow::_register_raw_input() {
//     // Below, we specify the keyboard and mouse as raw input devices. The
//     // RIDEV_NOLEGACY flag tells Windows not to generate legacy messages, like
//     // WM_KEYDOWN, while RIDEV_REMOVE unregisters the device from Windows' raw
//     // input.

//     std::array<::RAWINPUTDEVICE, 2> devices { };

//     devices[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
//     devices[0].usUsage     = HID_USAGE_GENERIC_KEYBOARD; // The keyboard
//     devices[0].dwFlags     = RIDEV_NOLEGACY;
//     devices[0].hwndTarget  = _window;

//     devices[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
//     devices[1].usUsage     = HID_USAGE_GENERIC_MOUSE; // And the mouse
//     devices[1].dwFlags     = RIDEV_NOLEGACY;
//     devices[1].hwndTarget  = _window;

//     // Attempt to register
//     auto const result = ::RegisterRawInputDevices(
//         devices.data(),
//         static_cast<::UINT>(devices.size()),
//         sizeof(::RAWINPUTDEVICE)
//     );

//     if(result == FALSE) {
//         auto const error = ::GetLastError();
//         BTX_CRITICAL("Failed to register raw x11 input with error: '{}'",
//                      std::system_category().message(static_cast<int>(error)));
//     }
//     else {
//         BTX_TRACE("Registered x11 raw input");
//     }
// }

// // =============================================================================
// void X11TargetWindow::_deregister_raw_input() {
//     // All of this is the same as when registering, except RIDEV_NOLEGACY is
//     // exchanged for RIDEV_REMOVE and providing nullptr instead of the window
//     // handle

//     std::array<::RAWINPUTDEVICE, 2> devices { };

//     devices[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
//     devices[0].usUsage     = HID_USAGE_GENERIC_KEYBOARD; // The keyboard
//     devices[0].dwFlags     = RIDEV_REMOVE;
//     devices[0].hwndTarget  = nullptr;

//     devices[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
//     devices[1].usUsage     = HID_USAGE_GENERIC_MOUSE; // And the mouse
//     devices[1].dwFlags     = RIDEV_REMOVE;
//     devices[1].hwndTarget  = nullptr;

//     // Attempt to deregister
//     auto const result = ::RegisterRawInputDevices(
//         devices.data(),
//         static_cast<::UINT>(devices.size()),
//         sizeof(::RAWINPUTDEVICE)
//     );

//     if(result == FALSE) {
//         auto const error = ::GetLastError();
//         BTX_CRITICAL("Failed to deregister raw input with error: '{}'",
//                      std::system_category().message(static_cast<int>(error)));
//     }
//     else {
//         BTX_TRACE("Deregistered x11 raw input");
//     }
// }

// // =============================================================================
// void X11TargetWindow::_restrict_cursor() {
//     // Retrieve the window's drawable surface
// 	::RECT client_area;
// 	::GetClientRect(_window, &client_area);

//     // Since GetClientRect() doesn't tell us anything about where the window
//     // is, we need to convert client_area to desktop coordinates
// 	::MapWindowPoints(
//         _window,
//         HWND_DESKTOP,
//         reinterpret_cast<::POINT *>(&client_area),
//         2
//     );

//     // Now we can ensure the cursor stays within the window
// 	::ClipCursor(&client_area);

//     // Run through all requests to show a cursor until ours is the last
//     while(::ShowCursor(FALSE) >= 0) { }
// }

// // =============================================================================
// void X11TargetWindow::_release_cursor() {
//     // Allow the cursor to travel outside the client space
//     ::ClipCursor(nullptr);

//     // Run through all requests to show a cursor until ours is the last
//     while(::ShowCursor(TRUE) < 0) { }
// }

// =============================================================================
void X11TargetWindow::_size_and_place() {
    BTX_TRACE("Target window size: {}x{}, position: {}x{}", _window_size.width,
                                                            _window_size.height,
                                                            _window_position.x,
                                                            _window_position.y);

    // auto const result = ::SetWindowPos(_window,
    //                                    nullptr, // hwnd insert after
    //                                    static_cast<int>(_window_position.x),
    //                                    static_cast<int>(_window_position.y),
    //                                    static_cast<int>(_window_size.width),
    //                                    static_cast<int>(_window_size.height),
    //                                    0); // flags

    // if(!SUCCEEDED(result)) {
    //     auto const error = ::GetLastError();
    //     BTX_CRITICAL("Failed to set x11 window position with error: '{}'",
    //                  std::system_category().message(static_cast<int>(error)));
    // }
}

// =============================================================================
void X11TargetWindow::_message_loop() {
    static ::XEvent event { };
    ::memset(&event, 0, sizeof(::XEvent));

    while(::XPending(_display) > 0) {
        ::XNextEvent(_display, &event);

        switch(event.type) {
            case DestroyNotify: {
                auto const *ev = reinterpret_cast<XDestroyWindowEvent*>(&event);
                if(ev->window == _window) {
                    EventBus::publish<WindowCloseEvent>({ });
                }
                break;
            }
        }
    }
}

// // =============================================================================
// ::LRESULT X11TargetWindow::_static_wndproc(::HWND hWnd, ::UINT uMsg,
//                                              ::WPARAM wParam, ::LPARAM lParam)
// {
//     static X11TargetWindow *target = nullptr;

//     if(uMsg == WM_NCCREATE) {
//         // Recover the "this" pointer which was passed as a parameter
//         // to CreateWindow
//         auto const *lpcs = reinterpret_cast<::LPCREATESTRUCT>(lParam);
//         target = static_cast<X11TargetWindow *>(lpcs->lpCreateParams);

//         // Put the value in a safe place for future use
//         ::SetWindowLongPtrW(hWnd, GWLP_USERDATA,
//                             reinterpret_cast<::LONG_PTR>(target));
//     }
//     else {
//         // Recover the "this" pointer from where our WM_NCCREATE handler
//         // stashed it.
//         target = reinterpret_cast<X11TargetWindow *>(
//             ::GetWindowLongPtrW(hWnd, GWLP_USERDATA)
//         );
//     }

//     if(target != nullptr) {
//         // Now that we have recovered our "this" pointer, let the
//         // member function finish the job.
//         return target->_inst_wndproc(hWnd, uMsg, wParam, lParam);
//     }

//     // We don't know what our "this" pointer is, so just do the default
//     // thing. Hopefully that doesn't break anything
//     return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
// }

// // =============================================================================
// ::LRESULT X11TargetWindow::_inst_wndproc(::HWND hWnd, ::UINT uMsg,
//                                            ::WPARAM wParam, ::LPARAM lParam)
// {
//     // BTX_ERROR("{}", _msg_map.translate(uMsg));

//     switch(uMsg) {
//         // This is the first message received in window close cascade. Note the
//         // early return so there's no call to ::DefWindowProc()
//         case WM_CLOSE:
//             ::DestroyWindow(hWnd);
//             ::UnregisterClassW(_window_class.lpszClassName,
//                                _window_class.hInstance);
//             EventBus::publish<WindowCloseEvent>({ });
//             return false;

//         // And this is the second message, but also the last one we have to
//         // handle explicitly.
//         case WM_DESTROY:
//             ::PostQuitMessage(0);
//             return false;

//         case WM_SIZE: {
//             uint32_t const width  = LOWORD(lParam);
//             uint32_t const height = HIWORD(lParam);

//             if(wParam == SIZE_MINIMIZED && !_minimized) {
//                 {
//                     std::unique_lock<std::mutex> write_lock(_size_mutex);
//                     _window_size.width = 0u;
//                     _window_size.height = 0u;
//                 }

//                 _minimized = true;

//                 BTX_TRACE("x11 window minimized");
//                 EventBus::publish<WindowMinimizeEvent>({ });
//             }
//             else if(wParam == SIZE_RESTORED && _minimized) {
//                 {
//                     std::unique_lock<std::mutex> write_lock(_size_mutex);
//                     _window_size.width = width;
//                     _window_size.height = height;
//                 }

//                 _minimized = false;

//                 BTX_TRACE("x11 window restored to {}x{}", width, height);
//                 EventBus::publish<WindowRestoreEvent>({ });
//             }
//             else if((width != _window_size.width)
//                     || (height != _window_size.height))
//             {
//                 {
//                     std::unique_lock<std::mutex> write_lock(_size_mutex);
//                     _window_size.width = width;
//                     _window_size.height = height;
//                 }

//                 BTX_TRACE("x11 window resized: {}x{}", width, height);
//                 EventBus::publish<WindowSizeEvent>({ });
//             }

//             break;
//         }

//         case WM_KEYDOWN: {
//             auto const translated = _keymap.translate(
//                 static_cast<const ::USHORT>(wParam)
//             );
//             EventBus::publish(KeyPressEvent { .code = translated });
//             break;
//         }

//         case WM_KEYUP: {
//             auto const translated = _keymap.translate(
//                 static_cast<const ::USHORT>(wParam)
//             );
//             EventBus::publish(KeyReleaseEvent { .code = translated });
//             break;
//         }

//         case WM_LBUTTONDOWN: {
//             EventBus::publish(MouseButtonPressEvent { .code = BTX_MB_LEFT });
//             break;
//         }

//         case WM_INPUT: {
//             // First call to GetRawInputData is just to get the size of the
//             // message so we can check it later
//             ::UINT message_size;
//             auto result = ::GetRawInputData(
//                 reinterpret_cast<::HRAWINPUT>(lParam),
//                 RID_INPUT,
//                 nullptr,
//                 &message_size,
//                 sizeof(::RAWINPUTHEADER)
//             );

//             if(result == ~0u) {
//                 BTX_CRITICAL("Failed to get x11 raw input message");
//                 break;
//             }

//             assert(message_size <= sizeof(::RAWINPUT));

//             // Get the actual message
//             result = ::GetRawInputData(
//                 reinterpret_cast<::HRAWINPUT>(lParam),
//                 RID_INPUT,
//                 _raw_msg,
//                 &message_size,
//                 sizeof(::RAWINPUTHEADER)
//             );

//             if(result != message_size) {
//                 BTX_CRITICAL(
//                     "X11 raw input expected {} bytes, got {} bytes instead",
//                     message_size, result
//                 );
//                 break;
//             }

//             // Begin by casting to the type we're going to parse
//             auto const *msg = reinterpret_cast<::RAWINPUT *>(_raw_msg);
//             if(msg->header.dwType == RIM_TYPEKEYBOARD) {
//                 // Discard "fake keys" which are part of an escaped sequence,
//                 // otherwise process the input
//                 if(msg->data.keyboard.VKey != 255u) {
//                     _parse_raw_keyboard(msg->data.keyboard);
//                 }
//             }
//             else if(msg->header.dwType == RIM_TYPEMOUSE) {
//                 _parse_raw_mouse(msg->data.mouse);
//             }

//             break;
//         }

//         default: break;
//     }

//     // Be sure to let Windows handle what hasn't already been handled
//     return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
// }

// // =============================================================================
// // Credit and big thanks to Stefan Reinalter for actually detailing the x11
// // raw input stuff for the rest of us mortals:

// // https://blog.molecular-matters.com/2011/09/05/properly-handling-keyboard-input/

// void X11TargetWindow::_parse_raw_keyboard(::RAWKEYBOARD const &raw) {
//     // We might need to modify the virtual key code
//     auto vkey = raw.VKey;

//     // correct left-hand / right-hand SHIFT
//     if(vkey == VK_SHIFT) {
//         vkey = static_cast<::USHORT>(
//             ::MapVirtualKeyW(raw.MakeCode, MAPVK_VSC_TO_VK_EX)
//         );
//     }

//     // Windows keys
//     else if(vkey == VK_LWIN) { vkey = BTX_KB_LEFT_SUPER; }
//     else if(vkey == VK_RWIN) { vkey = BTX_KB_RIGHT_SUPER; }

//     // And the remainder
//     else {
//         // e0 and e1 are escape sequences used for certain special
//         // keys, such as PRINT and PAUSE/BREAK.
//         // http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
//         bool const isE0 = ((raw.Flags & RI_KEY_E0) != 0);

//         if(isE0) {
//             switch(vkey) {
//                 // right-hand CONTROL and ALT have their e0 bit set
//                 case VK_CONTROL: vkey = BTX_KB_RIGHT_CTRL; break;
//                 case VK_MENU   : vkey = BTX_KB_RIGHT_ALT;  break;

//                 // NUMPAD ENTER has its e0 bit set
//                 case VK_RETURN : vkey = BTX_KB_NP_ENTER;   break;

//                 default: break;
//             }
//         }
//         else {
//             switch(vkey) {
//                 case VK_CONTROL: vkey = BTX_KB_LEFT_CTRL; break;
//                 case VK_MENU   : vkey = BTX_KB_LEFT_ALT;  break;

//                 // the standard INSERT, DELETE, HOME, END, PRIOR and
//                 // NEXT keys will always have their e0 bit set, but the
//                 // corresponding keys on the NUMPAD will not.
//                 case VK_INSERT: vkey = BTX_KB_NP_0;       break;
//                 case VK_DELETE: vkey = BTX_KB_NP_DECIMAL; break;
//                 case VK_HOME  : vkey = BTX_KB_NP_7;       break;
//                 case VK_END   : vkey = BTX_KB_NP_1;       break;
//                 case VK_PRIOR : vkey = BTX_KB_NP_9;       break;
//                 case VK_NEXT  : vkey = BTX_KB_NP_3;       break;
//                 // the standard arrow keys will always have their e0 bit
//                 // set, but the corresponding keys on the NUMPAD will
//                 // not.
//                 case VK_LEFT  : vkey = BTX_KB_NP_4; break;
//                 case VK_RIGHT : vkey = BTX_KB_NP_6; break;
//                 case VK_UP    : vkey = BTX_KB_NP_8; break;
//                 case VK_DOWN  : vkey = BTX_KB_NP_2; break;

//                 // NUMPAD 5 doesn't have its e0 bit set
//                 case VK_CLEAR : vkey = BTX_KB_NP_5; break;

//                 default: break;
//             }
//         }
//     }

//     // Now that we've got everything sorted, do a lookup for the key code.
//     auto const translated = _keymap.translate(vkey);

//     // a key can either produce a "make" or "break" scancode. this is used to
//     // differentiate between down-presses and releases
//     // http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
//     bool const is_release = ((raw.Flags & RI_KEY_BREAK) != 0);

//     if(is_release) {
//         EventBus::publish(KeyReleaseEvent { .code = translated });
//     }
//     else {
//         EventBus::publish(KeyPressEvent { .code = translated });
//     }
// }

// // =============================================================================
// void X11TargetWindow::_parse_raw_mouse(::RAWMOUSE const &raw) {
//     auto const button_data = raw.usButtonData;
//     auto const button_flags = raw.usButtonFlags;

//     // Left mouse button
//     if((button_flags & RI_MOUSE_LEFT_BUTTON_DOWN) != 0u) {
//         EventBus::publish(MouseButtonPressEvent { .code = BTX_MB_LEFT });
//     }
//     else if((button_flags & RI_MOUSE_LEFT_BUTTON_UP) != 0u) {
//         EventBus::publish(MouseButtonReleaseEvent { .code = BTX_MB_LEFT });
//     }

//     // Right mouse button
//     if((button_flags & RI_MOUSE_RIGHT_BUTTON_DOWN) != 0u) {
//         EventBus::publish(MouseButtonPressEvent { .code = BTX_MB_RIGHT });
//     }
//     else if((button_flags & RI_MOUSE_RIGHT_BUTTON_UP) != 0u) {
//         EventBus::publish(MouseButtonReleaseEvent { .code = BTX_MB_RIGHT });
//     }

//     // Middle mouse button
//     if((button_flags & RI_MOUSE_MIDDLE_BUTTON_DOWN) != 0u) {
//         EventBus::publish(MouseButtonPressEvent { .code = BTX_MB_MIDDLE });
//     }
//     else if((button_flags & RI_MOUSE_MIDDLE_BUTTON_UP) != 0u) {
//         EventBus::publish(MouseButtonReleaseEvent { .code = BTX_MB_MIDDLE });
//     }

//     // Mouse movement
//     if(raw.lLastX != 0 || raw.lLastY != 0) {
//         EventBus::publish(MouseMoveEvent {
//             .x_offset = raw.lLastX,
//             .y_offset = raw.lLastY
//         });
//     }

//     // Vertical mouse scroll
//     if((button_flags & RI_MOUSE_WHEEL) != 0u) {
//         EventBus::publish(MouseScrollEvent {
//             .vert_offset = static_cast<int32_t>(button_data),
//             .horiz_offset = 0
//         });
//     }

//     // Horizontal mouse scroll
//     if((button_flags & RI_MOUSE_HWHEEL) != 0u) {
//         EventBus::publish(MouseScrollEvent {
//             .vert_offset = 0,
//             .horiz_offset = static_cast<int32_t>(button_data)
//         });
//     }
// }

} // namespace btx

#endif // BTX_LINUX