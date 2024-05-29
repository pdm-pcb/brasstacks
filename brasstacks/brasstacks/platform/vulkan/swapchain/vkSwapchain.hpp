#ifndef BRASSTACKS_PLATFORM_VULKAN_SWAPCHAIN_VKSWAPCHAIN_HPP
#define BRASSTACKS_PLATFORM_VULKAN_SWAPCHAIN_VKSWAPCHAIN_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/config/RenderConfig.hpp"

namespace btx {

class vkSurface;
class vkFrameSync;
class vkImage;
class vkImageView;

class vkSwapchain final {
public:
    vkSwapchain();
    ~vkSwapchain();

    void create(vkSurface const &surface);
    void destroy();

    uint32_t get_next_image_index(vk::Semaphore const &semaphore);

    [[nodiscard]] bool present(vkFrameSync const &frame,
                               uint32_t const image_index);

    inline auto const & native()      const { return _handle; }
    inline auto const & size()        const { return _size; }
    inline auto const & offset()      const { return _offset; }
    inline auto const & images()      const { return _images; }
    inline auto const & image_views() const { return _image_views; }

    inline auto image_format() const { return _image_format.format; }
    inline auto aspect_ratio() const { return _aspect_ratio; }

    vkSwapchain(vkSwapchain &&) = delete;
    vkSwapchain(vkSwapchain const &) = delete;

    vkSwapchain & operator=(vkSwapchain &&) = delete;
    vkSwapchain & operator=(vkSwapchain const &) = delete;

private:
    vk::SwapchainKHR _handle;

    vk::Device _device;

    vk::SurfaceFormatKHR _image_format;
    vk::PresentModeKHR   _present_mode;

    std::vector<vkImage *>     _images;
    std::vector<vkImageView *> _image_views;

    RenderConfig::Size   _size;
    RenderConfig::Offset _offset;

    float _aspect_ratio;

    void _query_surface_capabilities(vk::SurfaceKHR const &surface);
    void _query_surface_format(vk::SurfaceKHR const &surface);
    void _query_surface_present_modes(vk::SurfaceKHR const &surface);

    vk::SwapchainCreateInfoKHR
    _populate_create_info(vk::SurfaceKHR const &surface);

    void _get_swapchain_images();
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_SWAPCHAIN_VKSWAPCHAIN_HPP