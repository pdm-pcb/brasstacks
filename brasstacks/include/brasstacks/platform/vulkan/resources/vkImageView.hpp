#ifndef BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKIMAGEVIEW_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKIMAGEVIEW_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkImageView final {
public:
    vkImageView();
    ~vkImageView();

    void create(vk::Image const &image,
                vk::Format const format,
                vk::ImageViewType const type,
                vk::ImageAspectFlags const aspect_flags);

    void destroy();

    auto const & native() const { return _handle; }

    vkImageView(vkImageView &&) = delete;
    vkImageView(const vkImageView &) = delete;

    vkImageView & operator=(vkImageView &&) = delete;
    vkImageView & operator=(const vkImageView &) = delete;

private:
    vk::ImageView _handle;
    vk::Device _device;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKIMAGEVIEW_HPP