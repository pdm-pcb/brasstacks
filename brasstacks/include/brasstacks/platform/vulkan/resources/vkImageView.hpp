#ifndef BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKIMAGEVIEW_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKIMAGEVIEW_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkImage;

class vkImageView final {
public:
    vkImageView();
    ~vkImageView() = default;

    void create(vkImage const &image,
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
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKIMAGEVIEW_HPP