#include "brasstacks/System/pch.hpp"
#include "brasstacks/Platform/Win32/Win32ConfigWindow.hpp"

#include "brasstacks/Engine/RenderConfig.hpp"
#include "brasstacks/Utility/config_constants.hpp"

bool CALLBACK set_font(::HWND child, ::LPARAM font) {
    ::SendMessage(child, WM_SETFONT, font, true);
    return true; // WM_SETFONT doesn't give SendMessage() anything to return
}

namespace btx {

void Win32ConfigWindow::run() {
    ::ShowWindow(_window, SW_SHOWNORMAL);

    ::MSG msg { };
    while(::GetMessage(&msg, nullptr, 0, 0) > 0)
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
}

::LRESULT CALLBACK Win32ConfigWindow::_wndproc(::HWND window,
                                               ::UINT msg,
                                               ::WPARAM wparam,
                                               ::LPARAM lparam) {
    Win32ConfigWindow *_this = nullptr;

    if(msg == WM_NCCREATE) {
        ::LPCREATESTRUCT lpstruct = reinterpret_cast<::LPCREATESTRUCT>(lparam);
        _this = static_cast<Win32ConfigWindow *>(lpstruct->lpCreateParams);

        ::SetWindowLongPtr(
            window,
            GWLP_USERDATA,
            reinterpret_cast<::LONG_PTR>(_this)
        );

        _this->_window = window;
    }
    else {
        _this = reinterpret_cast<Win32ConfigWindow *>(
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

::LRESULT Win32ConfigWindow::_message_handler(::HWND window,
                                              ::UINT msg,
                                              ::WPARAM wparam,
                                              ::LPARAM lparam) {
    switch(msg) {
        case WM_CREATE:
        {
            _populate_ui();
            _populate_gpu_list();
            _populate_res_list();
            _populate_api_list();
            _set_defaults();    // TODO: set defaults based on what's already
                                //       in RenderConfig, if anything. This
                                //       should imply storing actual information
                                //       in the rows for pattern matching/etc.

            ::HFONT segoe = ::CreateFont(
                16, 0, 0, 0,
                FW_REGULAR, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH,
                "Segoe UI"
            );

            ::EnumChildWindows(
                _window,
                reinterpret_cast<::WNDENUMPROC>(set_font),
                reinterpret_cast<::LPARAM>(segoe)
            );

            break;
        }

        case WM_COMMAND:
            switch(LOWORD(wparam))
            {
                case IDC_GPU_LIST:
                    if(HIWORD(wparam) == LBN_SELCHANGE) {
                        ::LRESULT selection = ::SendMessage(
                            _gpu_list,
                            LB_GETCURSEL,
                            0, 0
                        );

                        ::SendMessage(
                            _gpu_list,
                            LB_GETTEXT,
                            selection,
                            reinterpret_cast<::LPARAM>(_liststr)
                        );

                        ::SetWindowText(_chosen_gpu, _liststr);

                        ::LRESULT adapter_index = ::SendMessage(
                            _gpu_list,
                            LB_GETITEMDATA,
                            selection, 0
                        );

                        RenderConfig::adapter = static_cast<int32_t>(adapter_index);

                        BTX_ENGINE_TRACE(
                            "Adapter {} - {}",
                            adapter_index,
                            _adapters[adapter_index]
                        );
                    }
                    break;

                case IDC_RES_LIST:
                    if(HIWORD(wparam) == LBN_SELCHANGE) {
                        ::LRESULT selection = ::SendMessage(
                            _res_list,
                            LB_GETCURSEL,
                            0, 0
                        );

                        ::SendMessage(
                            _res_list,
                            LB_GETTEXT,
                            selection,
                            reinterpret_cast<::LPARAM>(_liststr)
                        );

                        ::SetWindowText(_chosen_res, _liststr);

                        ::LRESULT display_index = ::SendMessage(
                            _res_list,
                            LB_GETITEMDATA,
                            selection, 0
                        );

                        RenderConfig::window_x_res =
                            _display_settings[display_index].x_res;
                        RenderConfig::window_y_res =
                            _display_settings[display_index].y_res;
                        RenderConfig::refresh_rate =
                            _display_settings[display_index].refresh_rate;

                        BTX_ENGINE_TRACE(
                            "Display settings - {}x{}@{}hz",
                            RenderConfig::window_x_res,
                            RenderConfig::window_y_res,
                            RenderConfig::refresh_rate
                        );
                    }
                    break;

                case IDC_VSYNC:
                {
                    bool checked = ::IsDlgButtonChecked(_window, IDC_VSYNC);
                    ::CheckDlgButton(_window, IDC_VSYNC,
                                     checked ? BST_UNCHECKED : BST_CHECKED);

                    // use the negated state because we just changed it
                    RenderConfig::vsync_on = !checked;
                    break;
                }

                case IDC_FULLSCREEN:
                {
                    bool checked = ::IsDlgButtonChecked(_window, IDC_FULLSCREEN);
                    ::CheckDlgButton(_window, IDC_FULLSCREEN,
                                     checked ? BST_UNCHECKED : BST_CHECKED);

                    // negate it, since we just changed it
                    checked = !checked;
                    RenderConfig::fullscreen = checked;

                    BTX_ENGINE_TRACE("VSync {}", checked ? "on" : "off");
                    break;
                }

                case IDC_API:
                    if(HIWORD(wparam) == CBN_SELCHANGE) {
                        ::LRESULT selection = ::SendMessage(
                            _api_combobox,
                            CB_GETCURSEL,
                            0, 0
                        );

                        ::LRESULT api_index = ::SendMessage(
                            _api_combobox,
                            CB_GETITEMDATA,
                            selection, 0
                        );

                        RenderConfig::selected_api = api_index;

                        BTX_ENGINE_TRACE(
                            "{} selected",
                            RenderConfig::supported_apis[api_index]
                        );
                    }
                    break;

                case IDC_START:
                    ::SendMessage(window, WM_CLOSE, wparam, lparam);
                    break;

                case IDC_QUIT:
                    WindowCloseEvent event;
                    publish(EventType::WindowClosed, event);
                    ::SendMessage(window, WM_CLOSE, wparam, lparam);
                    break;
            }
            break;

        case WM_KEYDOWN:
            // TODO: add support for enter (and maybe space?) to start the
            //       application.
            //       Does this mean I'd have to write message handlers for the
            //       various controls such that they'd all forward enter on
            //       to the main window? We shall see.
            if(wparam == VK_ESCAPE) {
                WindowCloseEvent event;
                publish(EventType::WindowClosed, event);
                ::SendMessage(window, WM_CLOSE, wparam, lparam);
            }
            break;

        case WM_KEYUP:
            if(wparam == VK_RETURN) {
                ::SendMessage(window, WM_CLOSE, wparam, lparam);
            }
            break;

        case WM_CLOSE:
            ::ReleaseDC(_window, _device);
            ::DestroyWindow(_gpu_list_label);
            ::DestroyWindow(_gpu_list);
            ::DestroyWindow(_res_list_label);
            ::DestroyWindow(_res_list);
            ::DestroyWindow(_gpu_label);
            ::DestroyWindow(_chosen_gpu);
            ::DestroyWindow(_res_label);
            ::DestroyWindow(_chosen_res);
            ::DestroyWindow(_api_combobox);
            ::DestroyWindow(_vsync);
            ::DestroyWindow(_fullscreen);
            ::DestroyWindow(_start);
            ::DestroyWindow(_quit);
            ::DestroyWindow(_window);
            ::UnregisterClass(_classname, 0);
            break;

        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;

        default: break;
    }

    return ::DefWindowProc(window, msg, wparam, lparam);
}

void Win32ConfigWindow::_populate_ui() {
    _gpu_list_label = ::CreateWindow(
        "Static", "Graphics Adapters",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        WINDOW_PAD, WINDOW_PAD,
        LABEL_WIDTH, LABEL_HEIGHT,
        _window, 0, nullptr, nullptr
    );

    _gpu_list = ::CreateWindow(
        WC_LISTBOX, nullptr,
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | LBS_NOTIFY,
        WINDOW_PAD, WINDOW_PAD + LABEL_HEIGHT,
        LIST_WIDTH, LIST_HEIGHT / 2,
        _window, reinterpret_cast<::HMENU>(IDC_GPU_LIST),
        nullptr, nullptr
    );

    _res_list_label = ::CreateWindow(
        "Static", "Display Settings",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        LABEL_WIDTH + (WINDOW_PAD * 2), WINDOW_PAD,
        LABEL_WIDTH, LABEL_HEIGHT,
        _window, 0, nullptr, nullptr
    );

    _res_list = ::CreateWindow(
        WC_LISTBOX, nullptr,
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | LBS_NOTIFY,
        LIST_WIDTH + (WINDOW_PAD * 2), WINDOW_PAD + LABEL_HEIGHT,
        LIST_WIDTH, LIST_HEIGHT,
        _window, reinterpret_cast<::HMENU>(IDC_RES_LIST),
        nullptr, nullptr
    );

    _gpu_label = ::CreateWindow(
        "Static", "GPU: ",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        WINDOW_PAD, WINDOW_PAD + LABEL_HEIGHT + (LIST_HEIGHT / 2),
        LABEL_WIDTH / 4, LABEL_HEIGHT,
        _window, 0, nullptr, nullptr
    );

    _chosen_gpu = ::CreateWindow(
        "Static", "",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        WINDOW_PAD + (LABEL_WIDTH / 3),
        WINDOW_PAD + LABEL_HEIGHT + (LIST_HEIGHT / 2),
        static_cast<uint32_t>(LABEL_WIDTH * (2.0f / 3.0f)), LABEL_HEIGHT,
        _window, 0, nullptr, nullptr
    );

    _res_label = ::CreateWindow(
        "Static", "Resolution: ",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        WINDOW_PAD,
        WINDOW_PAD + (LABEL_HEIGHT * 2) + (LIST_HEIGHT / 2),
        LABEL_WIDTH, LABEL_HEIGHT,
        _window, 0, nullptr, nullptr
    );

    _chosen_res = ::CreateWindow(
        "Static", "",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        WINDOW_PAD + (LABEL_WIDTH / 3),
        WINDOW_PAD + (LABEL_HEIGHT * 2) + (LIST_HEIGHT / 2),
        static_cast<uint32_t>(LABEL_WIDTH * (2.0f / 3.0f)), LABEL_HEIGHT,
        _window, 0, nullptr, nullptr
    );

    _vsync = ::CreateWindow(
        "button", "VSync",
        WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
        WINDOW_PAD,
        WINDOW_PAD + (LABEL_HEIGHT * 3) + (LIST_HEIGHT / 2),
        (LABEL_WIDTH / 4), LABEL_HEIGHT,
        _window, reinterpret_cast<::HMENU>(IDC_VSYNC),
        nullptr, nullptr
    );

    ::CheckDlgButton(_window, IDC_VSYNC, BST_CHECKED);

    _fullscreen = ::CreateWindow(
        "button", "Full Screen",
        WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
        WINDOW_PAD,
        WINDOW_PAD + (LABEL_HEIGHT * 4) + (LIST_HEIGHT / 2),
        (LABEL_WIDTH / 3), LABEL_HEIGHT,
        _window, reinterpret_cast<::HMENU>(IDC_FULLSCREEN),
        nullptr, nullptr
    );

    ::CheckDlgButton(_window, IDC_FULLSCREEN, BST_UNCHECKED);
    
    _api_combobox = ::CreateWindow(
        "ComboBox", nullptr,
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
        WINDOW_PAD + (LABEL_WIDTH / 3),
        WINDOW_PAD + (LABEL_HEIGHT * 3) + (LIST_HEIGHT / 2),
        static_cast<uint32_t>(LABEL_WIDTH * (2.0f / 3.0f)),
        (LABEL_HEIGHT * static_cast<int>(
            RenderConfig::supported_apis.size()) * 2),
        _window, reinterpret_cast<::HMENU>(IDC_API),
        nullptr, nullptr
    );

    _start = ::CreateWindow("Button", "Start",
        WS_VISIBLE | WS_CHILD,
        WINDOW_X - (WINDOW_PAD * 2) - (BUTTON_WIDTH * 2),
        WINDOW_Y - WINDOW_PAD - BUTTON_HEIGHT,
        BUTTON_WIDTH, BUTTON_HEIGHT,
        _window, reinterpret_cast<::HMENU>(IDC_START),
        nullptr, nullptr
    );

    _quit = ::CreateWindow("Button", "Quit",
        WS_VISIBLE | WS_CHILD,
        WINDOW_X - WINDOW_PAD - BUTTON_WIDTH,
        WINDOW_Y - WINDOW_PAD - BUTTON_HEIGHT,
        BUTTON_WIDTH, BUTTON_HEIGHT,
        _window, reinterpret_cast<::HMENU>(IDC_QUIT),
        nullptr, nullptr
    );

    if(_gpu_list_label == nullptr) {
        ::MessageBox( nullptr, "GPU List label is null", "Error", MB_OK);
    }
    if(_gpu_list == nullptr) {
        ::MessageBox( nullptr, "GPU List is null", "Error", MB_OK);
    }
    if(_res_list_label == nullptr) {
        ::MessageBox( nullptr, "Resolution List label is null", "Error", MB_OK);
    }
    if(_res_list == nullptr) {
        ::MessageBox( nullptr, "Resolution List is null", "Error", MB_OK);
    }
    if(_gpu_label == nullptr) {
        ::MessageBox( nullptr, "GPU label is null", "Error", MB_OK);
    }
    if(_chosen_gpu == nullptr) {
        ::MessageBox( nullptr, "Chosen GPU label is null", "Error", MB_OK);
    }
    if(_res_label == nullptr) {
        ::MessageBox( nullptr, "Display label is null", "Error", MB_OK);
    }
    if(_chosen_res == nullptr) {
        ::MessageBox( nullptr, "Chosen Display label is null", "Error", MB_OK);
    }
    if(_vsync == nullptr) {
        ::MessageBox( nullptr, "VSync checkbox is null", "Error", MB_OK);
    }
    if(_fullscreen == nullptr) {
        ::MessageBox( nullptr, "Fullscreen checkbox is null", "Error", MB_OK);
    }
    if(_api_combobox == nullptr) {
        ::MessageBox( nullptr, "API combobox is null", "Error", MB_OK);
    }
    if(_start == nullptr) {
        ::MessageBox( nullptr, "Start button is null", "Error", MB_OK);
    }
    if(_quit == nullptr) {
        ::MessageBox( nullptr, "Quit button is null", "Error", MB_OK);
    }
}

void Win32ConfigWindow::_populate_gpu_list() {
    _device = ::GetDC(_window);
    if(_device == nullptr) {
        ::MessageBox(nullptr, "GetDC() failed", "Error", MB_OK);
    }

    ::DISPLAY_DEVICE dev { };
    dev.cb = sizeof(dev);

    ::DWORD dev_index = 0;
    ::CHAR  list_string[64] { };

    while(::EnumDisplayDevices(nullptr, dev_index++, &dev, 0)) {
        if(std::strcmp(dev.DeviceString, list_string) == 0) {
            continue;
        }

        std::strcpy(list_string, dev.DeviceString);

        ::LRESULT list_pos = ::SendMessage(
            _gpu_list, LB_ADDSTRING, 0,
            reinterpret_cast<::LPARAM>(dev.DeviceString)
        );

        _adapters.push_back(dev.DeviceID);

        ::SendMessage(_gpu_list, LB_SETITEMDATA, list_pos,
                      static_cast<::LPARAM>(_adapters.size() - 1));
    }
}

void Win32ConfigWindow::_populate_res_list() {
    ::DWORD res_index = 0;
    ::CHAR  list_string[64] { };
    ::CHAR    prev_string[64] { };

    ::DEVMODE mode { };
    mode.dmSize = sizeof(mode);

    while(::EnumDisplaySettings(nullptr, res_index++, &mode)) {
        std::sprintf(list_string, "%dx%d @ %dhz",
                     mode.dmPelsWidth, mode.dmPelsHeight,
                     mode.dmDisplayFrequency, mode.dmBitsPerPel
        );

        if(std::strcmp(list_string, prev_string) == 0) {
            continue;
        }

        std::strcpy(prev_string, list_string);

        ::LRESULT list_pos = ::SendMessage(
            _res_list, LB_ADDSTRING, 0,
            reinterpret_cast<LPARAM>(list_string)
        );

        _display_settings.push_back({
            static_cast<uint16_t>(mode.dmPelsWidth),
            static_cast<uint16_t>(mode.dmPelsHeight),
            static_cast<uint16_t>(mode.dmDisplayFrequency)
        });

        ::SendMessage(_res_list, LB_SETITEMDATA, list_pos,
                      static_cast<::LPARAM>(_display_settings.size() - 1));
    }
}

void Win32ConfigWindow::_populate_api_list() {
    size_t api_index = 0;

    for(const auto name : RenderConfig::supported_apis) {
        ::LRESULT list_pos = ::SendMessage(
            _api_combobox,
            CB_ADDSTRING, 0,
            reinterpret_cast<::LPARAM>(name)
        );

        ::SendMessage(_api_combobox, CB_SETITEMDATA, list_pos,
                      static_cast<::LPARAM>(api_index++));
    }
}

void Win32ConfigWindow::_set_defaults() {
    _select_gpu(0); // hopefully the default GPU
    _select_res(0); // lowest resolution and refresh rate
    _select_api(0); // OpenGL

    // disable the "future" features
    ::EnableWindow(_gpu_list, false);
    ::EnableWindow(_api_combobox, false);
}

void Win32ConfigWindow::_select_gpu(::LRESULT selection) {
    ::SendMessage(_gpu_list, LB_SETCURSEL, selection, 0);
    ::SendMessage(_gpu_list, LB_SETCARETINDEX, selection, FALSE);
    ::SendMessage(
        _window,
        WM_COMMAND,
        MAKEWPARAM(IDC_GPU_LIST, LBN_SELCHANGE),
        0
    );
}

void Win32ConfigWindow::_select_res(::LRESULT selection) {
    ::SendMessage(_res_list, LB_SETCURSEL, selection, 0);
    ::SendMessage(_res_list, LB_SETCARETINDEX, selection, FALSE);
    ::SendMessage(
        _window,
        WM_COMMAND,
        MAKEWPARAM(IDC_RES_LIST, LBN_SELCHANGE),
        0
    );
}

void Win32ConfigWindow::_select_api(::LRESULT selection) {
    ::SendMessage(_api_combobox, CB_SETCURSEL, selection, 0);
    ::SendMessage(
        _window,
        WM_COMMAND,
        MAKEWPARAM(IDC_API, CBN_SELCHANGE),
        0
    );
}

Win32ConfigWindow::Win32ConfigWindow() :
    ConfigWindow(),
    _window         { nullptr },
    _classname      { "Win32ConfigWindow" },
    _device         { nullptr },
    _gpu_list_label { nullptr },
    _gpu_list       { nullptr },
    _res_list_label { nullptr },
    _res_list       { nullptr },
    _gpu_label      { nullptr },
    _chosen_gpu     { nullptr },
    _res_label      { nullptr },
    _chosen_res     { nullptr },
    _vsync          { nullptr },
    _fullscreen     { nullptr },
    _api_combobox   { nullptr },
    _start          { nullptr },
    _quit           { nullptr },
    _liststr { }
{

    ::WNDCLASSEX wcex { };
    wcex.cbSize = sizeof(::WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = _wndproc;
    wcex.hbrBackground = ::GetSysColorBrush(COLOR_BTNFACE);
    wcex.lpszClassName = _classname;

    if(::RegisterClassEx(&wcex) == 0) {
        ::MessageBox(nullptr, "RegisterClassEx() failed", "Error", MB_OK);
    }

    _window = ::CreateWindowEx(
        0, _classname, "Brass Tacks Configuration",
        WS_POPUP | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_X, WINDOW_Y,
        nullptr, nullptr, 0, this
    );

    if(_window == nullptr) {
        ::MessageBox(nullptr, "CreateWindowEx() failed", "Error", MB_OK);
    }

    ::SetWindowPos(
        _window, nullptr,
        (GetSystemMetrics(SM_CXSCREEN) / 2) - (WINDOW_X / 2),
        (GetSystemMetrics(SM_CYSCREEN) / 2) - (WINDOW_Y / 2),
        WINDOW_X, WINDOW_Y,
        0
    );
}

} // namespace btx