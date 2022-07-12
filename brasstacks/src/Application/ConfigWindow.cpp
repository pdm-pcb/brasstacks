#include "brasstacks/System/pch.hpp"
#include "brasstacks/Application/ConfigWindow.hpp"

#ifdef __linux__
    // ...?
#elif defined(_WIN32)
    #include "brasstacks/Platform/Win32/Win32ConfigWindow.hpp"
#endif

namespace btx {

ConfigWindow * ConfigWindow::create() {
#ifdef __linux__
    // ...?
#elif defined(_WIN32)
    return new Win32ConfigWindow();
#endif
}

} // namespace btx