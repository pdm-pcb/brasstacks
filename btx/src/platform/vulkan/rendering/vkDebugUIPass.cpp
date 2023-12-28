#include "brasstacks/core.hpp"
#include "brasstacks/platform/vulkan/rendering/vkDebugUIPass.hpp"

#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/resources/vkImage.hpp"
#include "brasstacks/platform/vulkan/resources/vkImageView.hpp"

namespace btx {

vkDebugUIPass::vkDebugUIPass(vkDevice const &device, vk::Format const format,
                             vk::Extent2D const &extent) :
    vkRenderPass   { device },
    _extent        { extent },
    _color_format  { format },
    _color_buffers {  },
    _color_views   {  },
    _attachment_descriptions { },
    _color_attachments       { },
    _subpasses               { },
    _subpass_dependencies    { }
{
    _create_color_buffer();
    _init_attachments();
    _init_subpasses();

    vk::RenderPassCreateInfo const create_info {
        .attachmentCount = static_cast<uint32_t>(_attachment_descriptions.size()),
        .pAttachments    = _attachment_descriptions.data(),
        .subpassCount    = static_cast<uint32_t>(_subpasses.size()),
        .pSubpasses      = _subpasses.data(),
        .dependencyCount = static_cast<uint32_t>(_subpass_dependencies.size()),
        .pDependencies   = _subpass_dependencies.data()
    };

    this->create(create_info);
}

// =============================================================================
vkDebugUIPass::~vkDebugUIPass() {
    for(auto *view : _color_views) {
        delete view;
    }

    for(auto *buffer : _color_buffers) {
        delete buffer;
    }
}

// =============================================================================
void vkDebugUIPass::_create_color_buffer() {
    vkImage::ImageInfo const color_buffer_info {
        .type = vk::ImageType::e2D,
        .samples = vk::SampleCountFlagBits::e1,
        .usage_flags = (vk::ImageUsageFlagBits::eColorAttachment |
                        vk::ImageUsageFlagBits::eTransientAttachment),
        .memory_flags = vk::MemoryPropertyFlagBits::eDeviceLocal,
    };

    for(size_t i = 0; i < RenderConfig::swapchain_image_count; ++i) {
        _color_buffers.emplace_back(
            new vkImage(this->device(),
                        _extent,
                        _color_format,
                        color_buffer_info)
        );

        _color_views.emplace_back(
            new vkImageView(this->device(),
                            *_color_buffers.back(),
                            vk::ImageViewType::e2D,
                            vk::ImageAspectFlagBits::eColor)
        );
    }

    BTX_TRACE("Created debug UI color buffers");
}

// =============================================================================
void vkDebugUIPass::_init_attachments() {
    _attachment_descriptions = {{
        // color buffer attachment description
        .format         = _color_format,
        .samples        = vk::SampleCountFlagBits::e1,
        .loadOp         = vk::AttachmentLoadOp::eClear,
        .storeOp        = vk::AttachmentStoreOp::eDontCare,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout    = vk::ImageLayout::eColorAttachmentOptimal,
    }};

    _color_attachments = {{
        .attachment = 0u,
        .layout     = vk::ImageLayout::eColorAttachmentOptimal,
    }};
}

// =============================================================================
void vkDebugUIPass::_init_subpasses() {
    _subpasses = {{
        // This subpass is a graphical one
        .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,

        // ...Which has no input of any kind
        .inputAttachmentCount = 0u,
        .pInputAttachments    = nullptr,

        // But does have a single color attachment
        .colorAttachmentCount =
            static_cast<uint32_t>(_color_attachments.size()),
        .pColorAttachments   = _color_attachments.data(),

        .pResolveAttachments = nullptr,
        .pDepthStencilAttachment = nullptr,

        // As we've only got a single subpass, there's nothing to preserve
        // between subpasses
        .preserveAttachmentCount = 0u,
        .pPreserveAttachments    = nullptr,
    }};

    _subpass_dependencies = {{
        .srcSubpass      = VK_SUBPASS_EXTERNAL,
        .dstSubpass      = 0u,
        .srcStageMask    = vk::PipelineStageFlagBits::eBottomOfPipe,
        .dstStageMask    = vk::PipelineStageFlagBits::eColorAttachmentOutput,
        .srcAccessMask   = vk::AccessFlagBits::eMemoryRead,
        .dstAccessMask   = vk::AccessFlagBits::eColorAttachmentWrite,
        .dependencyFlags = vk::DependencyFlagBits::eByRegion
    }};
}

} // namespace btx