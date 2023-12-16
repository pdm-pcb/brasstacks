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
    vkFrame const & acquire_next_image_index();
    void present();

    auto image_format() const { return _image_format; }
    auto const & render_area() const { return _render_area; }

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

    uint32_t _next_image_index;
    std::queue<vk::Semaphore> _image_acquire_sems;
    std::vector<vkFrame *> _frames;

    void _query_surface_capabilities();
    void _query_surface_format();
    void _query_surface_present_modes();
    void _populate_create_info(vk::SwapchainCreateInfoKHR &create_info);
    void _get_swapchain_images();
    void _create_frame_data();
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKSWAPCHAIN_HPP