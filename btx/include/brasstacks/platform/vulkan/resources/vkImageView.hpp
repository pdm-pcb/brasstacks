#ifndef BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKIMAGEVIEW_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKIMAGEVIEW_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDevice;

class vkImageView final {
public:
    vkImageView(vkDevice const &device, vk::Image const &image,
                vk::Format const format, vk::ImageViewType const type,
                vk::ImageAspectFlags const aspect_flags);
    ~vkImageView();

    vkImageView() = delete;

    vkImageView(vkImageView &&) = delete;
    vkImageView(const vkImageView &) = delete;

    vkImageView & operator=(vkImageView &&) = delete;
    vkImageView & operator=(const vkImageView &) = delete;

private:
    vkDevice const &_device;
    vk::Image const &_image;

    vk::ImageView _handle;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKIMAGEVIEW_HPP