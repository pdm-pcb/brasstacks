#include "brasstacks/platform/vulkan/rendering/vkFramebuffer.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/rendering/vkRenderPass.hpp"
#include "brasstacks/platform/vulkan/resources/images/vkImage.hpp"

namespace btx {

// =============================================================================
vkFramebuffer::vkFramebuffer(vkDevice const &device,
                             vkRenderPass const &render_pass,
                             vk::Extent2D const &extent,
                             vkImage const &image) :
    _device { device }
{
    vk::FramebufferCreateInfo const create_info {
        .pNext           = nullptr,
        .flags           = { },
        .renderPass      = render_pass.native(),
        .attachmentCount = 1u,
        .pAttachments    = &image.view().native(),
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
    BTX_TRACE("Created framebuffer {:#x}",
              reinterpret_cast<uint64_t>(VkFramebuffer(_handle)));
}

// =============================================================================
vkFramebuffer::~vkFramebuffer() {
    BTX_TRACE("Destroying framebuffer {:#x}",
              reinterpret_cast<uint64_t>(VkFramebuffer(_handle)));

    _device.native().destroyFramebuffer(_handle);
}

} // namespace btx