/**
 * @file vkSwapchain.hpp
 * @brief An abstraction of the Vulkan swapchain and its various capabilities
 */

#ifndef BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKSWAPCHAIN_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKSWAPCHAIN_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkSurface;
class vkDevice;
class vkImage;
class vkImageView;
class vkFrameSync;

/**
 * @brief An abstraction of the Vulkan swapchain and its various capabilities
 *
 * Renderer uses this swapchain abstraction to keep track of the vkSurface
 * it's created and which image index is next to bet written to.
 */
class vkSwapchain final {
public:

    /**
     * @brief Construct the vkSwapchain object.
     * @param surface The surface about which this swapchain is concerned
     * @param device An established Vulkan logical device
     */
    vkSwapchain(vkDevice const &device, vkSurface const &surface);

    ~vkSwapchain();

    /**
     * @brief Hands a particular semaphore off to the driver to signal when the
     * swapchain image corresponding with the returned index is acquired
     * @param semaphore The semaphore to be signaled once the image is acquired
     * @return uint32_t The index of the acquired image
     */
    uint32_t acquire_image_index(vk::Semaphore const &semaphore);

    /**
     * @brief Ask the swapchain to queue a given image for presentation
     * @param frame The synchronization structures associated with the image
     * to be presented
     * @param image_index The index of the swapchain image to be presented
     */
    void present(vkFrameSync const &frame, uint32_t const image_index);

    /**
     * @brief Return the swapchain's image format
     * @return vk::Format
     */
    inline auto image_format() const { return _image_format.format; }

    /**
     * @brief Return a vector of images from this swapchain
     * @return std::vector<vkImage *> const&
     */
    inline auto const & images() const { return _images; }

    /**
     * @brief Return a vector of image views for the images from this swapchain
     * @return std::vector<vkImageView *> const&
     */
    inline auto const & image_views() const { return _image_views; }

    vkSwapchain() = delete;

    vkSwapchain(vkSwapchain &&) = delete;
    vkSwapchain(vkSwapchain const &) = delete;

    vkSwapchain & operator=(vkSwapchain &&) = delete;
    vkSwapchain & operator=(vkSwapchain const &) = delete;

private:
    /**
     * @brief The Vulkan logical device from which this swapchain was created
     */
    vkDevice const &_device;

    /**
     * @brief The swapchain images have the same format as the related surface
     */
    vk::SurfaceFormatKHR _image_format;

    /**
     * @brief The configured mode for the presentation engine
     */
    vk::PresentModeKHR _present_mode;

    /**
     * @brief The native Vulkan handle
     */
    vk::SwapchainKHR _handle;

    /**
     * @brief A list of images from this swapchain
     */
    std::vector<vkImage *> _images;

    /**
     * @brief A list of image views for the images from this swapchain
     */
    std::vector<vkImageView *> _image_views;

    /**
     * @brief Query various surface capabilities, like supported image counts,
     * resolutions, and array layers
     * @param surface The associated Vulkan surface
     */
    void _query_surface_capabilities(vk::SurfaceKHR const &surface);

    /**
     * @brief Query for the surface's image format and color space
     * @param surface The associated Vulkan surface
     */
    void _query_surface_format(vk::SurfaceKHR const &surface);

    /**
     * @brief Query for supported presentation engine modes
     * @param surface The associated Vulkan surface
     */
    void _query_surface_present_modes(vk::SurfaceKHR const &surface);

    /**
     * @brief Fill and return a Vulkan swapchain create info structure
     * @param surface The Vulkan surface this swapchain is configured to match
     * @return vk::SwapchainCreateInfoKHR The completed create info struct
     */
    vk::SwapchainCreateInfoKHR
    _populate_create_info(vk::SurfaceKHR const &surface);

    /**
     * @brief Retrieves images from an established swapchain, then creates
     * image views for them
     */
    void _get_swapchain_images();

    void _recreate();
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKSWAPCHAIN_HPP