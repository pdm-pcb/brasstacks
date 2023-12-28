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

/**
 * @brief Simple wrapper for a Vulkan framebuffer
 */
class vkFramebuffer final {
public:
    vkFramebuffer(vkDevice const &device, vkRenderPass const &render_pass,
                  vk::Extent2D const &extent,
                  std::vector<vk::ImageView> const &attachments);

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