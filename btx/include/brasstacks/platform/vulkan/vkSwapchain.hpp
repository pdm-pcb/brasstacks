#ifndef BRASSTACKS_PLATFORM_VULKAN_VKSWAPCHAIN_HPP
#define BRASSTACKS_PLATFORM_VULKAN_VKSWAPCHAIN_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkPhysicalDevice;
class vkSurface;
class vkDevice;
class vkImageView;
class vkFrame;

class vkSwapchain final {
public:
    void acquire_next_image_index(vkFrame &frame);
    void present(vkFrame const &frame);

    inline auto const & render_area()  const { return _render_area;  }
    inline auto         image_format() const { return _image_format; }
    inline auto         image_count()  const { return _image_views.size(); }

    vkSwapchain(vkPhysicalDevice const &adapter, vkSurface const &surface,
                vkDevice const &device);
    ~vkSwapchain();

    vkSwapchain() = delete;

    vkSwapchain(vkSwapchain &&) = delete;
    vkSwapchain(vkSwapchain const &) = delete;

    vkSwapchain & operator=(vkSwapchain &&) = delete;
    vkSwapchain & operator=(vkSwapchain const &) = delete;

private:
    vkPhysicalDevice const &_adapter;
    vkSurface        const &_surface;
    vkDevice         const &_device;

    vk::Rect2D         _render_area;
    vk::Format         _image_format;
    vk::ColorSpaceKHR  _color_space;
    vk::PresentModeKHR _present_mode;

    vk::SwapchainKHR _handle;
    std::vector<vkImageView *> _image_views;

    void _query_surface_capabilities();
    void _query_surface_format();
    void _query_surface_present_modes();
    void _populate_create_info(vk::SwapchainCreateInfoKHR &create_info);
    void _get_images();
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_VKSWAPCHAIN_HPP