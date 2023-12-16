#ifndef BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKIMAGE_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKIMAGE_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDevice;

class vkImage final {
public:
    inline auto const & device() const { return _device; }
    inline auto const & native() const { return _handle; }
    inline auto format()         const { return _format; }

    // For accessing existing swapchain images
    vkImage(vkDevice const &device, vk::Image const &handle,
            vk::Format const format);

    ~vkImage();

    vkImage() = delete;

    vkImage(vkImage &&) = delete;
    vkImage(const vkImage &) = delete;

    vkImage & operator=(vkImage &&) = delete;
    vkImage & operator=(const vkImage &) = delete;

private:
    vkDevice const &_device;

    vk::Image            _handle;
    vk::Format           _format;
    vk::ImageView        _view;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKIMAGE_HPP