#include "brasstacks/core.hpp"
#include "brasstacks/platform/vulkan/rendering/vkSurface.hpp"

#include "brasstacks/platform/vulkan/vkInstance.hpp"

namespace btx {

vkSurface::vkSurface(vk::Win32SurfaceCreateInfoKHR const &create_info) {
    // Attempt to create, then check
    auto const result = vkInstance::native().createWin32SurfaceKHR(create_info);
    if(result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Unable to create Win32 KHR surface: '{}'",
                     vk::to_string(result.result));
        return;
    }

    // And assign
    _handle = result.value;
    BTX_TRACE("Created Vulkan surface {}", _handle);
}

vkSurface::~vkSurface() {
    BTX_TRACE("Destroying Vulkan surface {}", _handle);
    vkInstance::native().destroy(_handle);
    _handle = nullptr;
}

} // namespace btx