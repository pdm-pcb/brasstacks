#ifndef BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKFRAMEBUFFER_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKFRAMEBUFFER_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDevice;
class vkRenderPass;

class vkFramebuffer final {
public:
    inline auto const & native() const { return _handle; }

    vkFramebuffer(vkDevice const &device, vkRenderPass const &render_pass,
                  vk::Extent2D const &extent, vk::ImageView const &image_view);
    ~vkFramebuffer();

    vkFramebuffer() = delete;

    vkFramebuffer(vkFramebuffer &&) = delete;
    vkFramebuffer(const vkFramebuffer &) = delete;

    vkFramebuffer & operator=(vkFramebuffer &&) = delete;
    vkFramebuffer & operator=(const vkFramebuffer &) = delete;

private:
    vkDevice const &_device;

    vk::Framebuffer _handle;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKFRAMEBUFFER_HPP