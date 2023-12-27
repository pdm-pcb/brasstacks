#include "brasstacks/core.hpp"
#include "brasstacks/platform/vulkan/rendering/vkColorDepthPass.hpp"

#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/resources/vkImage.hpp"

namespace btx {

vkColorDepthPass::vkColorDepthPass(vkPhysicalDevice const &physical_device,
                                   vkDevice const &device,
                                   vk::Format const format,
                                   vk::SampleCountFlagBits const msaa_samples) :
    vkRenderPass { device },
    _depth_format { vk::Format::eUndefined },
    _color_buffer { nullptr },
    _depth_buffer { nullptr }
{
    _find_depth_stencil_format(physical_device);

    // Describe the color buffer attachment
    vk::AttachmentDescription const color_buffer_desc {
        .flags          = { },
        .format         = format,
        .samples        = msaa_samples,
        .loadOp         = vk::AttachmentLoadOp::eClear,
        .storeOp        = vk::AttachmentStoreOp::eStore,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout    = vk::ImageLayout::ePresentSrcKHR,
    };

	// We have one subpass. This subpass has one color attachment. While
    // executing this subpass, the attachment will be in attachment optimal
    // layout.
    vk::AttachmentReference const color_buffer_ref {
        .attachment = 0,
        .layout = vk::ImageLayout::eColorAttachmentOptimal,
    };

	// We will end up with two transitions.
	// The first one happens right before we start subpass #0, where
	// eUndefined is transitioned into eColorAttachmentOptimal.
	// The final layout in the render pass attachment states ePresentSrcKHR, so
	// we will get a final transition from eColorAttachmentOptimal to
    // ePresetSrcKHR.
    vk::SubpassDescription const subpass_desc {
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
    vk::SubpassDependency const subpass_dep {
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
	vk::RenderPassCreateInfo const create_info {
        .pNext           = nullptr,
        .flags           = { },
        .attachmentCount = 1u,
        .pAttachments    = &color_buffer_desc,
        .subpassCount    = 1u,
        .pSubpasses      = &subpass_desc,
        .dependencyCount = 1u,
        .pDependencies   = &subpass_dep,
    };

    this->create(create_info);
}

// =============================================================================
void vkColorDepthPass::_find_depth_stencil_format(
    vkPhysicalDevice const &physical_device)
{
    const std::vector<vk::Format> depth_options {
        vk::Format::eD32Sfloat,
        vk::Format::eD32SfloatS8Uint,
        vk::Format::eD24UnormS8Uint
    };

    for(auto const& option : depth_options) {
        auto props = physical_device.native().getFormatProperties(option);
        if(props.optimalTilingFeatures &
           vk::FormatFeatureFlagBits::eDepthStencilAttachment)
        {
            BTX_TRACE("Using depth stencil format {}", vk::to_string(option));
            _depth_format = option;
            return;
        }
    }

    BTX_CRITICAL("Unable to find suitable depth stencil format");
}

// =============================================================================
void
vkColorDepthPass::_init_color_buffer(vk::Format const format,
                                     vk::Extent2D const extent,
                                     vk::SampleCountFlagBits const samples)
{
    // _color_buffer = new vkImage(_device, extent, format);
    // _color_buffer->create(
    //     vk::ImageType::e2D,
    //     samples,
    //     (
    //         vk::ImageUsageFlagBits::eColorAttachment |
    //         vk::ImageUsageFlagBits::eTransientAttachment // ??????????????????????????????????????????????????????????????????
    //     ),
    //     vk::MemoryPropertyFlagBits::eDeviceLocal
    // );

    // ImageTools::create_view(
    //     _color_buffer,
    //     vk::ImageViewType::e2D,
    //     vk::ImageAspectFlagBits::eColor
    // );
}

} // namespace btx