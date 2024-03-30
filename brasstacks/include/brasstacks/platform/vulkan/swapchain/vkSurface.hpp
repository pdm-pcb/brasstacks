#ifndef BRASSTACKS_PLATFORM_VULKAN_SWAPCHAIN_VKSURFACE_HPP
#define BRASSTACKS_PLATFORM_VULKAN_SWAPCHAIN_VKSURFACE_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkSurface final {
public:
    vkSurface();
    ~vkSurface();

#ifdef BTX_LINUX
    void create(vk::XlibSurfaceCreateInfoKHR const &create_info);
#elif BTX_WINDOWS
    void create(vk::Win32SurfaceCreateInfoKHR const &create_info);
#endif // BTX platform

    void destroy();

    inline auto const & native() const { return _handle; }

    vkSurface(vkSurface &&) = delete;
    vkSurface(const vkSurface &) = delete;

    vkSurface & operator=(vkSurface &&) = delete;
    vkSurface & operator=(const vkSurface &) = delete;

private:
    vk::SurfaceKHR _handle;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_SWAPCHAIN_VKSURFACE_HPP