#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/passes/vkColorDepthPass.hpp"

#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/swapchain/vkSwapchain.hpp"
#include "brasstacks/platform/vulkan/resources/vkImage.hpp"
#include "brasstacks/platform/vulkan/resources/vkImageView.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkPipeline.hpp"

namespace btx {

vkColorDepthPass::vkColorDepthPass(Renderer const &renderer,
                                   bool const present) :
    vkRenderPassBase  { renderer },
    _depth_format { vk::Format::eUndefined },
    _msaa_samples {
        vkPipeline::samples_to_flag(btx::RenderConfig::msaa_samples)
    },
    _color_buffers           { },
    _color_views             { },
    _depth_buffers           { },
    _depth_views             { },
    _attachment_descriptions { },
    _color_attachments       { },
    _depth_attachment        { },
    _resolve_attachments     { },
    _subpasses               { },
    _subpass_dependencies    { }
{
    _find_depth_stencil_format();
    _init_attachments(present);
    _init_subpasses();
    _create_color_buffers();
    _create_depth_buffers();

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
vkColorDepthPass::~vkColorDepthPass() {
    _destroy_depth_buffers();
    _destroy_color_buffers();
}

// =============================================================================
void vkColorDepthPass::destroy_swapchain_resources() {
    _destroy_depth_buffers();
    _destroy_color_buffers();
}

// =============================================================================
void vkColorDepthPass::recreate_swapchain_resources() {
    _create_color_buffers();
    _create_depth_buffers();
}

// =============================================================================
void vkColorDepthPass::_find_depth_stencil_format() {
    const std::vector<vk::Format> depth_options {
        vk::Format::eD32Sfloat,
        vk::Format::eD32SfloatS8Uint,
        vk::Format::eD24UnormS8Uint
    };

    for(auto const& option : depth_options) {
        auto props = vkPhysicalDevice::native().getFormatProperties(option);
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
void vkColorDepthPass::_init_attachments(bool const present) {
    _attachment_descriptions = {{
        // color buffer (msaa) attachment description
        .format  = swapchain().image_format(),
        .samples = _msaa_samples,
        .loadOp  = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eDontCare,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout = vk::ImageLayout::eColorAttachmentOptimal,
    },
    {   // depth buffer attachment description
        .format  = _depth_format,
        .samples = _msaa_samples,
        .loadOp  = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eDontCare,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    },
    {   // final presentation/resolve attachment
        .format  = swapchain().image_format(),
        .samples = vk::SampleCountFlagBits::e1,
        .loadOp  = vk::AttachmentLoadOp::eDontCare,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout = (present ? vk::ImageLayout::ePresentSrcKHR :
                                  vk::ImageLayout::eColorAttachmentOptimal),
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

    auto const image_count = swapchain().images().size();
    for(size_t i = 0; i < image_count; ++i) {
        _color_buffers.emplace_back(
            new vkImage(this->device(),
                        { .width = swapchain().size().width,
                          .height = swapchain().size().height },
                        swapchain().image_format(),
                        color_buffer_info)
        );

        _color_views.emplace_back(
            new vkImageView(this->device(),
                            *_color_buffers.back(),
                            vk::ImageViewType::e2D,
                            vk::ImageAspectFlagBits::eColor)
        );
    }

    BTX_TRACE("Created color/depth color buffers");
}

// =============================================================================
void vkColorDepthPass::_create_depth_buffers() {
    vkImage::ImageInfo const depth_stencil_info {
        .type         = vk::ImageType::e2D,
        .samples      = _msaa_samples,
        .usage_flags  = vk::ImageUsageFlagBits::eDepthStencilAttachment,
        .memory_flags = vk::MemoryPropertyFlagBits::eDeviceLocal,
    };

    auto const image_count = swapchain().images().size();
    for(size_t i = 0; i < image_count; ++i) {
        _depth_buffers.emplace_back(
            new vkImage(this->device(),
                        { .width = swapchain().size().width,
                          .height = swapchain().size().height },
                        _depth_format,
                        depth_stencil_info)
        );

        _depth_views.emplace_back(
            new vkImageView(this->device(),
                            *_depth_buffers.back(),
                            vk::ImageViewType::e2D,
                            vk::ImageAspectFlagBits::eDepth)
        );
    }

    BTX_TRACE("Created color/depth depth buffers");
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
void vkColorDepthPass::_destroy_depth_buffers() {
    for(auto *view : _depth_views) {
        delete view;
    }

    _depth_views.clear();

    for(auto *buffer : _depth_buffers) {
        delete buffer;
    }

    _depth_buffers.clear();
}

} // namespace btx