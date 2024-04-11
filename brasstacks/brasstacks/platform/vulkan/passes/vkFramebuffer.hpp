#ifndef BRASSTACKS_PLATFORM_VULKAN_PASSES_VKFRAMEBUFFER_HPP
#define BRASSTACKS_PLATFORM_VULKAN_PASSES_VKFRAMEBUFFER_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/config/RenderConfig.hpp"

namespace btx {

class vkRenderPassBase;

class vkFramebuffer final {
public:
    vkFramebuffer();
    ~vkFramebuffer();

    void create(vkRenderPassBase const &render_pass,
                RenderConfig::Size const &size,
                std::vector<vk::ImageView> const &attachments);
    void destroy();

    inline auto const & native() const { return _handle; }

    vkFramebuffer(vkFramebuffer &&rhs);
    vkFramebuffer(const vkFramebuffer &) = delete;

    vkFramebuffer & operator=(vkFramebuffer &&) = delete;
    vkFramebuffer & operator=(const vkFramebuffer &) = delete;

private:
    vk::Framebuffer _handle;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_PASSES_VKFRAMEBUFFER_HPP