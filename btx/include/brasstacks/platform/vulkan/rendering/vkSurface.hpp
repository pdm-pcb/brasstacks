/**
 * @file vkSurface.hpp
 * @brief A wrapper class for a Vulkan surface.
 */

#ifndef BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKSURFACE_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKSURFACE_HPP

#include "brasstacks/pch.hpp"

namespace btx {

/**
 * @brief A wrapper class for a Vulkan surface.
 *
 * This class handles the creation and destruction of a platform-specific
 * Vulkan surface for Renderer to draw to.
 */
class vkSurface final {
public:
#if defined(BTX_LINUX)
    /**
     * @brief Construct the X11 vkSurface object.
     * @param instance An established Vulkan instance
     * @param create_info Surface creation details
     */
    vkSurface(vk::XlibSurfaceCreatInfoKHR const &create_info);

#elif defined(BTX_WINDOWS)
    /**
     * @brief Construct the win32 vkSurface object.
     * @param create_info Surface creation details
     */
    vkSurface(vk::Win32SurfaceCreateInfoKHR const &create_info);

#endif // BTX platform

    ~vkSurface();

    /**
     * @brief Return the native Vulkan surface handle.
     * @return vk::SurfaceKHR const&
     */
    inline auto const & native() const { return _handle; }

    vkSurface() = delete;

    vkSurface(vkSurface &&) = delete;
    vkSurface(const vkSurface &) = delete;

    vkSurface & operator=(vkSurface &&) = delete;
    vkSurface & operator=(const vkSurface &) = delete;

private:
    /**
     * @brief The native Vulkan handle for this surface.
     */
    vk::SurfaceKHR _handle;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKSURFACE_HPP