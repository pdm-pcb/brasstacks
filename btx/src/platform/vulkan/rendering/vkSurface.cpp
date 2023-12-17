#include "brasstacks/platform/vulkan/rendering/vkSurface.hpp"
#include "brasstacks/platform/vulkan/vkInstance.hpp"

namespace btx {

vkSurface::vkSurface(vkInstance const &instance,
                     vk::Win32SurfaceCreateInfoKHR const &create_info) :
_instance { instance }
{
    // Attempt to create, then check
    auto const result = _instance.native().createWin32SurfaceKHR(create_info);
    if(result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Unable to create Win32 KHR surface: '{}'",
                     vk::to_string(result.result));
        return;
    }

    // And assign
    _handle = result.value;
    BTX_TRACE("Created Vulkan surface {:#x}",
              reinterpret_cast<uint64_t>(::VkSurfaceKHR(_handle)));
}

vkSurface::~vkSurface() {
    BTX_TRACE("Destroying Vulkan surface {:#x}",
              reinterpret_cast<uint64_t>(::VkSurfaceKHR(_handle)));

    _instance.native().destroy(_handle);
}

} // namespace btx