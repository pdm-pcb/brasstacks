#include "brasstacks/core.hpp"
#include "brasstacks/platform/vulkan/rendering/vkUILayerPass.hpp"

#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/resources/vkImage.hpp"

namespace btx {

vkUILayerPass::vkUILayerPass(vkDevice const &device, vk::Format const format) :
    vkRenderPass { device }
{
    vk::AttachmentDescription const attachment {
        .flags          = { },
        .format         = format,
        .samples        = vk::SampleCountFlagBits::e1,
        .loadOp         = vk::AttachmentLoadOp::eLoad,
        .storeOp        = vk::AttachmentStoreOp::eStore,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eColorAttachmentOptimal,
        .finalLayout    = vk::ImageLayout::ePresentSrcKHR,
    };

    vk::AttachmentReference const color_attachment {
        .attachment = 0u,
        .layout = vk::ImageLayout::eColorAttachmentOptimal,
    };

    vk::SubpassDescription const subpass {
        .flags                   = { },
        .pipelineBindPoint       = vk::PipelineBindPoint::eGraphics,
        .inputAttachmentCount    = 0u,
        .pInputAttachments       = nullptr,
        .colorAttachmentCount    = 1u,
        .pColorAttachments       = &color_attachment,
        .pResolveAttachments     = nullptr,
        .pDepthStencilAttachment = nullptr,
        .preserveAttachmentCount = VK_FALSE,
        .pPreserveAttachments    = nullptr,
    };

    vk::SubpassDependency const dependency {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0u,
        .srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
        .dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
        .srcAccessMask = { }, // or vk::AccessFlagBits::eColorAttachmentWrite
        .dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite,
        .dependencyFlags = { },
    };

    vk::RenderPassCreateInfo const create_info {
        .pNext = nullptr,
        .flags = { },
        .attachmentCount = 1u,
        .pAttachments = &attachment,
        .subpassCount = 1u,
        .pSubpasses = &subpass,
        .dependencyCount = 1u,
        .pDependencies = &dependency,
    };

    this->_create(create_info);
}

} // namespace btx