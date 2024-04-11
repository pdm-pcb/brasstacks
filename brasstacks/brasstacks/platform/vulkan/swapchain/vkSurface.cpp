#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/swapchain/vkSurface.hpp"

#include "brasstacks/platform/vulkan/vkInstance.hpp"

namespace btx {

// =============================================================================
vkSurface::vkSurface() :
    _handle { nullptr }
{ }

// =============================================================================
vkSurface::~vkSurface() {
    if(_handle) {
        destroy();
    }
}

// =============================================================================
#ifdef BTX_LINUX
    void vkSurface::create(vk::XlibSurfaceCreateInfoKHR const &create_info) {
        if(_handle) {
            BTX_CRITICAL("Surface {} already exists", _handle);
            return;
        }

        _handle = vkInstance::native().createXlibSurfaceKHR(create_info);
        BTX_TRACE("Created Vulkan surface {}", _handle);
    }
#elif BTX_WINDOWS
    void vkSurface::create(vk::Win32SurfaceCreateInfoKHR const &create_info) {
        if(_handle) {
            BTX_CRITICAL("Surface {} already exists", _handle);
            return;
        }

        _handle = vkInstance::native().createWin32SurfaceKHR(create_info);
        BTX_TRACE("Created Vulkan surface {}", _handle);
    }
#endif

// =============================================================================
void vkSurface::destroy() {
    BTX_TRACE("Destroying Vulkan surface {}", _handle);
    vkInstance::native().destroy(_handle);
    _handle = nullptr;
}

} // namespace btx