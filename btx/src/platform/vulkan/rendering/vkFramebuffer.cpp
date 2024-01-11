#include "brasstacks/brasstacks.hpp"

#include "brasstacks/platform/vulkan/rendering/vkFramebuffer.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/rendering/vkRenderPass.hpp"
#include "brasstacks/platform/vulkan/resources/vkImageView.hpp"

namespace btx {

// =============================================================================
vkFramebuffer::vkFramebuffer(vkDevice const &device,
                             vkRenderPass const &render_pass,
                             vk::Extent2D const &extent,
                             std::vector<vk::ImageView> const &attachments) :
    _device { device }
{
    vk::FramebufferCreateInfo const create_info {
        .pNext           = nullptr,
        .flags           = { },
        .renderPass      = render_pass.native(),
        .attachmentCount = static_cast<uint32_t>(attachments.size()),
        .pAttachments    = attachments.data(),
        .width           = extent.width,
        .height          = extent.height,
        .layers          = 1u,
    };

    auto const result = _device.native().createFramebuffer(create_info);
    if(result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to create framebuffer: '{}'",
                     vk::to_string(result.result));
        return;
    }

    _handle = result.value;
    BTX_TRACE("Created framebuffer {} for render pass {} with extent {}x{}",
              _handle, render_pass.native(), extent.width, extent.height);
}

// =============================================================================
vkFramebuffer::~vkFramebuffer() {
    BTX_TRACE("Destroying framebuffer {}", _handle);

    _device.native().destroyFramebuffer(_handle);
}

} // namespace btx