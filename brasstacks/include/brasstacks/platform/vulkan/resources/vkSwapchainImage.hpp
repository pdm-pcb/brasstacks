#ifndef BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKSWAPCHAINIMAGE_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKSWAPCHAINIMAGE_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkSwapchainImage final {
public:
    vkSwapchainImage();
    ~vkSwapchainImage() = default;

    void create(vk::Image const &handle, vk::Format const format);

    inline auto const & native() const { return _handle; }
    inline auto format()         const { return _format; }

    vkSwapchainImage(vkSwapchainImage &&) = delete;
    vkSwapchainImage(const vkSwapchainImage &) = delete;

    vkSwapchainImage & operator=(vkSwapchainImage &&) = delete;
    vkSwapchainImage & operator=(const vkSwapchainImage &) = delete;

private:
    vk::Image  _handle;
    vk::Format _format;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKSWAPCHAINIMAGE_HPP