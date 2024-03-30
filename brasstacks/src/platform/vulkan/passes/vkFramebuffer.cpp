#include "brasstacks/brasstacks.hpp"

#include "brasstacks/platform/vulkan/passes/vkFramebuffer.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/passes/vkRenderPassBase.hpp"
#include "brasstacks/platform/vulkan/resources/vkImageView.hpp"

namespace btx {

// =============================================================================
vkFramebuffer::vkFramebuffer(vkRenderPassBase const &render_pass,
                             RenderConfig::Size const &size,
                             std::vector<vk::ImageView> const &attachments)
{
    vk::FramebufferCreateInfo const create_info {
        .pNext           = nullptr,
        .flags           = { },
        .renderPass      = render_pass.native(),
        .attachmentCount = static_cast<uint32_t>(attachments.size()),
        .pAttachments    = attachments.data(),
        .width           = size.width,
        .height          = size.height,
        .layers          = 1u,
    };

    _handle = Renderer::device().native().createFramebuffer(create_info);
    BTX_TRACE("Created framebuffer {} for render pass {} with extent {}x{}",
              _handle, render_pass.native(), size.width, size.height);
}

// =============================================================================
vkFramebuffer::~vkFramebuffer() {
    BTX_TRACE("Destroying framebuffer {}", _handle);

    Renderer::device().native().destroyFramebuffer(_handle);
}

} // namespace btx