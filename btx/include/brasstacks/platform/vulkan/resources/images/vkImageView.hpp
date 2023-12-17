/**
 * @file vkImageView.hpp
 * @brief A wrapper class for Vulkan image views
 */

#ifndef BRASSTACKS_PLATFORM_VULKAN_RESOURCES_IMAGES_VKIMAGEVIEW_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RESOURCES_IMAGES_VKIMAGEVIEW_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDevice;

/**
 * @brief A wrapper class for Vulkan image views
 */
class vkImageView final {
public:

    /**
     * @brief Construct the vkImageView object.
     * @param device An established Vulkan logical device
     * @param image The image for which this view is being created
     * @param format The associated image's format
     * @param type The image type, eg 2D, 3D, Cube
     * @param aspect_flags Image flags, eg color, depth, stencil
     */
    vkImageView(vkDevice const &device, vk::Image const &image,
                vk::Format const format, vk::ImageViewType const type,
                vk::ImageAspectFlags const aspect_flags);

    ~vkImageView();

    /**
     * @brief Return the native Vulkan handle
     * @return vk::ImageView const&
     */
    auto const & native() const { return _handle; }

    vkImageView() = delete;

    vkImageView(vkImageView &&) = delete;
    vkImageView(const vkImageView &) = delete;

    vkImageView & operator=(vkImageView &&) = delete;
    vkImageView & operator=(const vkImageView &) = delete;

private:
    /**
     * @brief The logical device that created this image view
     */
    vkDevice const &_device;

    /**
     * @brief The native Vulkan handle
     */
    vk::ImageView _handle;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RESOURCES_IMAGES_VKIMAGE_HPP