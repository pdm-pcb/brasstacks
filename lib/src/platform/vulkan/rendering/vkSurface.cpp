#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/rendering/vkSurface.hpp"

#include "brasstacks/platform/vulkan/vkInstance.hpp"

namespace btx {

#ifdef BTX_LINUX

vkSurface::vkSurface(vk::XlibSurfaceCreateInfoKHR const &create_info) {
    _handle = vkInstance::native().createXlibSurfaceKHR(create_info);
    BTX_TRACE("Created Vulkan surface {}", _handle);
}

#elif BTX_WINDOWS

vkSurface::vkSurface(vk::Win32SurfaceCreateInfoKHR const &create_info) {
    _handle = vkInstance::native().createWin32SurfaceKHR(create_info);
    BTX_TRACE("Created Vulkan surface {}", _handle);
}

#endif

vkSurface::~vkSurface() {
    BTX_TRACE("Destroying Vulkan surface {}", _handle);
    vkInstance::native().destroy(_handle);
    _handle = nullptr;
}

} // namespace btx