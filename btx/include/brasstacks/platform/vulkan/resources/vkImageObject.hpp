#ifndef BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKIMAGEOBJECT_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKIMAGEOBJECT_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkLogicalDevice;

class vkImageObject final {
public:
    void create_view(vk::ImageViewType const type);

    vkImageObject(vkLogicalDevice  const &device,
                  vk::Image const &handle,
                  vk::Format const format,
                  vk::ImageLayout const layout,
                  vk::ImageAspectFlags const flags);
    ~vkImageObject();

    vkImageObject() = delete;

private:
    vk::Image            _handle;
    vk::Format           _format;
    vk::ImageLayout      _layout;
    vk::ImageAspectFlags _flags;
    vk::ImageView        _view;

    vkLogicalDevice const &_device;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKIMAGEOBJECT_HPP