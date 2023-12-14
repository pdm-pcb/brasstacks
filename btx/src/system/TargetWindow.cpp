#include "brasstacks/system/TargetWindow.hpp"
#include "brasstacks/platform/x11/X11TargetWindow.hpp"
#include "brasstacks/platform/win32/Win32TargetWindow.hpp"

namespace btx {

TargetWindow * TargetWindow::create(std::string_view const app_name,
                                    Dimensions const &dimensions,
                                    Position const &position)
{

#if defined(BTX_LINUX)
    return new X11TargetWindow(app_name);
#elif defined(BTX_WINDOWS)
    return new Win32TargetWindow(app_name, dimensions, position);
#endif // BTX platform

}

} // namespace btx