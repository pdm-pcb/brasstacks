#ifndef BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKCOLORBUFFER_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKCOLORBUFFER_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDevice;
class vkSwapchain;
class vkImage;

class vkColorBuffer final {
public:
    vkColorBuffer(vkDevice const &device, vkSwapchain const &swapchain);
    ~vkColorBuffer();

    vkColorBuffer() = delete;

    vkColorBuffer(vkColorBuffer &&) = delete;
    vkColorBuffer(const vkColorBuffer &) = delete;

    vkColorBuffer & operator=(vkColorBuffer &&) = delete;
    vkColorBuffer & operator=(const vkColorBuffer &) = delete;

private:
    vkImage *_image;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKCOLORBUFFER_HPP