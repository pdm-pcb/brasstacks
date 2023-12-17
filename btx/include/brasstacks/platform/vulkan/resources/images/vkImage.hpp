#ifndef BRASSTACKS_PLATFORM_VULKAN_RESOURCES_IMAGES_VKIMAGE_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RESOURCES_IMAGES_VKIMAGE_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/platform/vulkan/resources/images/vkImageView.hpp"

namespace btx {

class vkDevice;

class vkImage final {
public:
    vkImage(vkDevice const &device, vk::Image const &handle,
            vk::Format const format);

    ~vkImage();

    inline auto const & native() const { return _handle;  }
    inline auto const & view()   const { return *_view;   }
    inline auto format()         const { return _format;  }

    vkImage() = delete;

    vkImage(vkImage &&) = delete;
    vkImage(const vkImage &) = delete;

    vkImage & operator=(vkImage &&) = delete;
    vkImage & operator=(const vkImage &) = delete;

private:
    vkDevice const &_device;

    vk::Image   _handle;
    vkImageView *_view;
    vk::Format  _format;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RESOURCES_IMAGES_VKIMAGE_HPP