#ifndef BRASSTACKS_PLATFORM_VULKAN_VKSWAPCHAIN_HPP
#define BRASSTACKS_PLATFORM_VULKAN_VKSWAPCHAIN_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkPhysicalDevice;
class vkDevice;
class vkImageObject;

class vkSwapchain final {
public:
    inline auto const& render_area()  { return _render_area;  }
    inline auto const& images()       { return _images;       }
    inline auto        image_format() { return _image_format; }

    vkSwapchain(vk::PhysicalDevice const &adapter,
                    vk::Device const &device,
                    vk::SurfaceKHR const &surface);
    ~vkSwapchain();

    vkSwapchain() = delete;

    vkSwapchain(vkSwapchain &&) = delete;
    vkSwapchain(vkSwapchain const &) = delete;

    vkSwapchain & operator=(vkSwapchain &&) = delete;
    vkSwapchain & operator=(vkSwapchain const &) = delete;

private:
    vk::Rect2D         _render_area;
    vk::Format         _image_format;
    vk::ColorSpaceKHR  _color_space;
    vk::PresentModeKHR _present_mode;

    vk::SwapchainKHR _handle;
    std::vector<vkImageObject *> _images;

    vk::PhysicalDevice const &_adapter;
    vk::Device         const &_device;
    vk::SurfaceKHR     const &_surface;

    void _query_surface_capabilities();
    void _query_surface_format();
    void _query_surface_present_modes();
    void _populate_create_info(vk::SwapchainCreateInfoKHR &create_info);
    void _get_images();
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_VKSWAPCHAIN_HPP