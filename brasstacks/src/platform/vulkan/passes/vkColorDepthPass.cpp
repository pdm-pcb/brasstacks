#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/passes/vkColorDepthPass.hpp"

#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/swapchain/vkSwapchain.hpp"
#include "brasstacks/platform/vulkan/resources/vkImage.hpp"
#include "brasstacks/platform/vulkan/resources/vkImageView.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkPipeline.hpp"

namespace btx {

vkColorDepthPass::vkColorDepthPass() :
    vkRenderPassBase { },
    _depth_format { vk::Format::eUndefined },
    _msaa_samples {
        vkPipeline::samples_to_flag(btx::RenderConfig::msaa_samples)
    },
    _color_buffers           { },
    _color_views             { },
    _depth_buffer            { nullptr },
    _depth_view              { nullptr },
    _attachment_descriptions { },
    _color_attachments       { },
    _depth_attachment        { },
    _resolve_attachments     { },
    _subpasses               { },
    _subpass_dependencies    { }
{
    _find_depth_stencil_format();
    _init_attachment_details();
    _init_subpasses();

    vk::RenderPassCreateInfo const create_info {
        .attachmentCount = static_cast<uint32_t>(
                                _attachment_descriptions.size()),
        .pAttachments    = _attachment_descriptions.data(),
        .subpassCount    = static_cast<uint32_t>(_subpasses.size()),
        .pSubpasses      = _subpasses.data(),
        .dependencyCount = static_cast<uint32_t>(_subpass_dependencies.size()),
        .pDependencies   = _subpass_dependencies.data()
    };

    this->_create(create_info);
}

// =============================================================================
void vkColorDepthPass::destroy_swapchain_resources() {
    _destroy_depth_buffer();
    _destroy_color_buffers();
}

// =============================================================================
void vkColorDepthPass::create_swapchain_resources() {
    _create_color_buffers();
    _create_depth_buffer();
}

// =============================================================================
void vkColorDepthPass::_find_depth_stencil_format() {
    static std::array<vk::Format, 2> const depth_formats {
        vk::Format::eD32SfloatS8Uint, // One of these two will always be
        vk::Format::eD24UnormS8Uint,  // supported, according to the Guide.
    };

    for(auto const format : depth_formats) {
        auto props = vkPhysicalDevice::native().getFormatProperties(format);
        if(props.optimalTilingFeatures &
           vk::FormatFeatureFlagBits::eDepthStencilAttachment)
        {
            BTX_TRACE("Using depth stencil format {}", vk::to_string(format));
            _depth_format = format;
            return;
        }
    }

    BTX_CRITICAL("Unable to find suitable depth stencil format");
}

// =============================================================================
void vkColorDepthPass::_init_attachment_details() {
    _attachment_descriptions = {{
        // color buffer (msaa) attachment description
        .format         = Renderer::swapchain().image_format(),
        .samples        = _msaa_samples,
        .loadOp         = vk::AttachmentLoadOp::eClear,
        .storeOp        = vk::AttachmentStoreOp::eDontCare,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout    = vk::ImageLayout::eColorAttachmentOptimal,
    },
    {   // depth buffer attachment description
        .format         = _depth_format,
        .samples        = _msaa_samples,
        .loadOp         = vk::AttachmentLoadOp::eClear,
        .storeOp        = vk::AttachmentStoreOp::eDontCare,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    },
    {   // final presentation/resolve attachment
        .format         = Renderer::swapchain().image_format(),
        .samples        = vk::SampleCountFlagBits::e1,
        .loadOp         = vk::AttachmentLoadOp::eDontCare,
        .storeOp        = vk::AttachmentStoreOp::eStore,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout    = vk::ImageLayout::ePresentSrcKHR,
    }};

    _color_attachments = {{
        .attachment = 0u,
        .layout = vk::ImageLayout::eColorAttachmentOptimal,
    }};

    _depth_attachment = {
        .attachment = 1u,
        .layout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    };

    _resolve_attachments = {{
        .attachment = 2u,
        .layout = vk::ImageLayout::eColorAttachmentOptimal,
    }};
}

// =============================================================================
void vkColorDepthPass::_init_subpasses() {
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

        // With whatever MSAA samples we've got
        .pResolveAttachments = _resolve_attachments.data(),

        // With a depth stencil
        .pDepthStencilAttachment = &_depth_attachment,

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
    },
    {
        .srcSubpass      = VK_SUBPASS_EXTERNAL,
        .dstSubpass      = 0u,
        .srcStageMask    = (vk::PipelineStageFlagBits::eEarlyFragmentTests |
                            vk::PipelineStageFlagBits::eLateFragmentTests),
        .dstStageMask    = (vk::PipelineStageFlagBits::eEarlyFragmentTests |
                            vk::PipelineStageFlagBits::eLateFragmentTests),
        .srcAccessMask   = vk::AccessFlagBits::eNone,
        .dstAccessMask   = vk::AccessFlagBits::eDepthStencilAttachmentWrite,
        .dependencyFlags = vk::DependencyFlagBits::eByRegion
    }};
}

// =============================================================================
void vkColorDepthPass::_create_color_buffers() {
    vkImage::ImageInfo const color_buffer_info {
        .type         = vk::ImageType::e2D,
        .samples      = _msaa_samples,
        .usage_flags  = (vk::ImageUsageFlagBits::eColorAttachment |
                         vk::ImageUsageFlagBits::eTransientAttachment),
        .memory_flags = vk::MemoryPropertyFlagBits::eDeviceLocal,
    };

    auto const image_count = Renderer::swapchain().images().size();
    for(size_t i = 0; i < image_count; ++i) {
        _color_buffers.emplace_back(
            new vkImage(
                vk::Extent2D {
                    .width = Renderer::swapchain().size().width,
                    .height = Renderer::swapchain().size().height
                },
                Renderer::swapchain().image_format(),
                color_buffer_info
            )
        );

        _color_views.emplace_back(
            new vkImageView(
                *_color_buffers.back(),
                vk::ImageViewType::e2D,
                vk::ImageAspectFlagBits::eColor
            )
        );
    }

    BTX_TRACE("Created {} color buffers for color/depth pass", image_count);
}

// =============================================================================
void vkColorDepthPass::_create_depth_buffer() {
    vkImage::ImageInfo const depth_stencil_info {
        .type         = vk::ImageType::e2D,
        .samples      = _msaa_samples,
        .usage_flags  = vk::ImageUsageFlagBits::eDepthStencilAttachment,
        .memory_flags = vk::MemoryPropertyFlagBits::eDeviceLocal,
    };

    _depth_buffer = new vkImage(
        vk::Extent2D {
            .width = Renderer::swapchain().size().width,
            .height = Renderer::swapchain().size().height
        },
        _depth_format,
        depth_stencil_info
    );

    _depth_view =  new vkImageView(
        *_depth_buffer,
        vk::ImageViewType::e2D,
        vk::ImageAspectFlagBits::eDepth
    );

    BTX_TRACE("Created depth buffer for color/depth pass");
}

// =============================================================================
void vkColorDepthPass::_destroy_color_buffers() {
    for(auto *view : _color_views) {
        delete view;
    }

    _color_views.clear();

    for(auto *buffer : _color_buffers) {
        delete buffer;
    }

    _color_buffers.clear();
}

// =============================================================================
void vkColorDepthPass::_destroy_depth_buffer() {
    delete _depth_view;
    delete _depth_buffer;
}

} // namespace btx