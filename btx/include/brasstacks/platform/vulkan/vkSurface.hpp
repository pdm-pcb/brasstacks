#ifndef BRASSTACKS_PLATFORM_VULKAN_VKSURFACE_HPP
#define BRASSTACKS_PLATFORM_VULKAN_VKSURFACE_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkInstance;

class vkSurface final {
public:
    inline auto const & native() const { return _handle; }

#if defined(BTX_LINUX)

    vkSurface(vkInstance const &instance,
              vk::XlibSurfaceCreatInfoKHR const &create_info);

#elif defined(BTX_WINDOWS)

    vkSurface(vkInstance const &instance,
              vk::Win32SurfaceCreateInfoKHR const &create_info);

#endif // BTX platform

    ~vkSurface();

    vkSurface() = delete;

    vkSurface(vkSurface &&) = delete;
    vkSurface(const vkSurface &) = delete;

    vkSurface & operator=(vkSurface &&) = delete;
    vkSurface & operator=(const vkSurface &) = delete;

private:
    vkInstance const &_instance;
    vk::SurfaceKHR _handle;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_VKSURFACE_HPP