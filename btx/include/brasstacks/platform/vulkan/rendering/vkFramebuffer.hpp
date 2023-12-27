/**
 * @file vkFramebuffer.hpp
 * @brief Simple wrapper for a Vulkan framebuffer
 */

#ifndef BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKFRAMEBUFFER_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKFRAMEBUFFER_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDevice;
class vkRenderPass;
class vkImageView;

/**
 * @brief Simple wrapper for a Vulkan framebuffer
 */
class vkFramebuffer final {
public:

    /**
     * @brief Construct the vkFramebuffer object.
     * @param device An established Vulkan logical device
     * @param render_pass Render pass associated with this framebuffer
     * @param extent Drawable dimensions of this framebuffer
     * @param image The image this framebuffer will represent
     */
    vkFramebuffer(vkDevice const &device, vkRenderPass const &render_pass,
                  vk::Extent2D const &extent, vkImageView const &view);

    ~vkFramebuffer();

    /**
     * @brief Return the native Vulkan handle
     * @return auto const&
     */
    inline auto const & native() const { return _handle; }

    vkFramebuffer() = delete;

    vkFramebuffer(vkFramebuffer &&) = delete;
    vkFramebuffer(const vkFramebuffer &) = delete;

    vkFramebuffer & operator=(vkFramebuffer &&) = delete;
    vkFramebuffer & operator=(const vkFramebuffer &) = delete;

private:
    /**
     * @brief The Vulkan logical device used to create this framebuffer
     */
    vkDevice const &_device;

    /**
     * @brief Native Vulkan handle
     */
    vk::Framebuffer _handle;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKFRAMEBUFFER_HPP