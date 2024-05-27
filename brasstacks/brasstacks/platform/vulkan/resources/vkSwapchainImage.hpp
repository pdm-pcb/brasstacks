#ifndef BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKSWAPCHAINIMAGE_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKSWAPCHAINIMAGE_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkSwapchainImage final {
public:
    vkSwapchainImage();
    ~vkSwapchainImage() = default;

    void create(vk::Image const &handle, vk::Format const format);
    void destroy();

    void transition_layout(vkCmdBuffer const &cmd_buffer,
                           vk::ImageLayout const old_layout,
                           vk::ImageLayout const new_layout);

    inline auto const & native() const { return _handle; }
    inline auto format()         const { return _format; }

    vkSwapchainImage(vkSwapchainImage &&rhs);
    vkSwapchainImage(const vkSwapchainImage &) = delete;

    vkSwapchainImage & operator=(vkSwapchainImage &&) = delete;
    vkSwapchainImage & operator=(const vkSwapchainImage &) = delete;

private:
    vk::Image  _handle;
    vk::Format _format;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKSWAPCHAINIMAGE_HPP