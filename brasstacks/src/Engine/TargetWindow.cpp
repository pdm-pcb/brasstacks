#include "brasstacks/System/pch.hpp"
#include "brasstacks/Engine/TargetWindow.hpp"

#ifdef __linux__
    #include "brasstacks/Platform/X11/X11TargetWindow.hpp"
#elif defined(_WIN32)
    #include "brasstacks/Platform/Win32/Win32TargetWindow.hpp"
#endif

namespace btx {

TargetWindow * TargetWindow::create() {
#ifdef __linux__
    return new X11TargetWindow;
#elif defined(_WIN32)
    ::SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    return new Win32TargetWindow;
#endif
}

} // namespace btx