#ifndef BRASSTACKS_SYSTEM_WINDOW_TARGETWINDOW_HPP
#define BRASSTACKS_SYSTEM_WINDOW_TARGETWINDOW_HPP

#if defined(BTX_LINUX)

    #include "brasstacks/platform/x11/X11TargetWindow.hpp"
    namespace btx {
        using TargetWindow = X11TargetWindow;
    } // namespace btx

#elif defined(BTX_WINDOWS)

    #include "brasstacks/platform/win32/Win32TargetWindow.hpp"
    namespace btx {
        using TargetWindow = Win32TargetWindow;
    } // namespace btx

#endif // BTX platform

#endif // BRASSTACKS_SYSTEM_WINDOW_TARGETWINDOW_HPP