#include "brasstacks/platform/vulkan/rendering/vkRenderPass.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"

namespace btx {

vkRenderPass::vkRenderPass(vkDevice const &device, vk::Format const format) :
    _device { device }
{
    // Describe the color buffer attachment
    vk::AttachmentDescription color_buffer_desc {
        .flags = { },

        // Format of the back buffer
        .format = format,

        // No multisampling
        .samples = vk::SampleCountFlagBits::e1,

        // When starting the frame, we want tiles to be cleared
        .loadOp = vk::AttachmentLoadOp::eClear,

        // When ending the frame, we want tiles to be written out
        .storeOp = vk::AttachmentStoreOp::eStore,

        // Don't care about stencil since we're not using it
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,

        // The image layout will be undefined when the render pass begins
        .initialLayout = vk::ImageLayout::eUndefined,

        // After the render pass is complete, we want to presnt
        .finalLayout = vk::ImageLayout::ePresentSrcKHR,
    };

	// We have one subpass. This subpass has one color attachment. While
    // executing this subpass, the attachment will be in attachment optimal
    // layout.
    vk::AttachmentReference color_buffer_ref {
        .attachment = 0,
        .layout = vk::ImageLayout::eColorAttachmentOptimal,
    };

	// We will end up with two transitions.
	// The first one happens right before we start subpass #0, where
	// eUndefined is transitioned into eColorAttachmentOptimal.
	// The final layout in the render pass attachment states ePresentSrcKHR, so
	// we will get a final transition from eColorAttachmentOptimal to
    // ePresetSrcKHR.
    vk::SubpassDescription subpass_desc {
        .flags                   = { },
        .pipelineBindPoint       = vk::PipelineBindPoint::eGraphics,
        .inputAttachmentCount    = 0u,
        .pInputAttachments       = nullptr,
        .colorAttachmentCount    = 1u,
        .pColorAttachments       = &color_buffer_ref,
        .pResolveAttachments     = nullptr,
        .pDepthStencilAttachment = nullptr,
        .preserveAttachmentCount = 0u,
        .pPreserveAttachments    = nullptr,
    };

	// Create a dependency to external events.
	// We need to wait for the WSI semaphore to signal.
	// Only pipeline stages which depend on eColorAttachmentOutput will
	// actually wait for the semaphore, so we must also wait for that pipeline stage.
    vk::SubpassDependency subpass_dep {
        .srcSubpass   = VK_SUBPASS_EXTERNAL,
        .dstSubpass   = 0u,
        .srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
        .dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,

         // Since we changed the image layout, we need to make the memory
         // visible to color attachment to modify.
        .srcAccessMask = { },
        .dstAccessMask = (vk::AccessFlagBits::eColorAttachmentRead |
                          vk::AccessFlagBits::eColorAttachmentWrite),
        .dependencyFlags = { },
    };

	// Finally, create the renderpass.
	vk::RenderPassCreateInfo render_pass_info {
        .pNext           = nullptr,
        .flags           = { },
        .attachmentCount = 1u,
        .pAttachments    = &color_buffer_desc,
        .subpassCount    = 1u,
        .pSubpasses      = &subpass_desc,
        .dependencyCount = 1u,
        .pDependencies   = &subpass_dep,
    };

    auto const result = _device.native().createRenderPass(render_pass_info);
    if(result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to create render pass: '{}'",
                     vk::to_string(result.result));
        return;
    }

    _handle = result.value;
}

vkRenderPass::~vkRenderPass() {
    _device.native().destroyRenderPass(_handle);
}

} // namespace btx