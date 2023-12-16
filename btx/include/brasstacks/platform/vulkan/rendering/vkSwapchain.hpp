#ifndef BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKSWAPCHAIN_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKSWAPCHAIN_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkPhysicalDevice;
class vkSurface;
class vkDevice;
class vkImageView;
class vkFrame;

class vkSwapchain final {
public:
    uint32_t acquire_next_image_index(vk::Semaphore const &semaphore);
    void present(vkFrame const &frame, uint32_t const image_index);

    inline auto image_format()        const { return _image_format; }
    inline auto const & image_views() const { return _image_views; }
    inline auto const & extent()      const { return _render_area.extent; }
    inline auto const & offset()      const { return _render_area.offset; }
    inline auto const & render_area() const { return _render_area; }

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
    void _get_swapchain_images();
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKSWAPCHAIN_HPP