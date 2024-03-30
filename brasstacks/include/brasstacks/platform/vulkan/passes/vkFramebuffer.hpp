/**
 * @file passes/vkFramebuffer.hpp
 * @brief Simple wrapper for a Vulkan framebuffer
 */

#ifndef BRASSTACKS_PLATFORM_VULKAN_PASSES_VKFRAMEBUFFER_HPP
#define BRASSTACKS_PLATFORM_VULKAN_PASSES_VKFRAMEBUFFER_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/config/RenderConfig.hpp"

namespace btx {

class vkRenderPassBase;

/**
 * @brief Simple wrapper for a Vulkan framebuffer
 */
class vkFramebuffer final {
public:
    vkFramebuffer(vkRenderPassBase const &render_pass,
                  RenderConfig::Size const &size,
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
     * @brief Native Vulkan handle
     */
    vk::Framebuffer _handle;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_PASSES_VKFRAMEBUFFER_HPP