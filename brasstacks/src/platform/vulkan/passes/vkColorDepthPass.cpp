#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/passes/vkColorDepthPass.hpp"

#include "brasstacks/platform/vulkan/vkInstance.hpp"
#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/swapchain/vkSwapchain.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkPipeline.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorPool.hpp"

#include "brasstacks/platform/imgui/UIOverlay.hpp"

namespace btx {

vkColorDepthPass::vkColorDepthPass() :
    vkRenderPassBase { },
    _depth_format { vk::Format::eUndefined },
    _msaa_samples { vkPipeline::samples_to_flag(RenderConfig::current_msaa) },
    _color_buffers           { },
    _color_views             { },
    _depth_buffer            { std::make_unique<vkImage>() },
    _depth_view              { std::make_unique<vkImageView>() },
    _attachment_descriptions { },
    _color_attachments       { },
    _depth_attachment        { },
    _resolve_attachments     { },
    _subpasses               { },
    _subpass_dependencies    { }
{ }

// =============================================================================
void vkColorDepthPass::create() {
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
void vkColorDepthPass::destroy() {
    this->_destroy();
}

// =============================================================================
void vkColorDepthPass::destroy_swapchain_resources() {
    UIOverlay::destroy_swapchain_resources();

    _destroy_depth_buffer();
    _destroy_color_buffers();
}

// =============================================================================
void vkColorDepthPass::create_swapchain_resources() {
    _create_color_buffers();
    _create_depth_buffer();

    UIOverlay::create_swapchain_resources(*this);
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
    BTX_TRACE("Creating {} color buffers for color/depth pass", image_count);

    if(_color_buffers.size() != image_count
       && _color_views.size() != image_count)
    {
        _color_buffers.clear();
        _color_views.clear();

        _color_buffers.reserve(image_count);
        _color_views.reserve(image_count);

        // Fill in the to be defined images with in-place construction
        std::generate_n(
            std::back_inserter(_color_buffers),
            _color_buffers.capacity(),
            []() {
                return std::make_unique<vkImage>();
            }
        );

        // And likewise the views
        std::generate_n(
            std::back_inserter(_color_views),
            _color_views.capacity(),
            []() {
                return std::make_unique<vkImageView>();
            }
        );
    }

    for(size_t i = 0; i < image_count; ++i) {
        _color_buffers[i]->create(
            vk::Extent2D {
                .width = Renderer::swapchain().size().width,
                .height = Renderer::swapchain().size().height
            },
            Renderer::swapchain().image_format(),
            color_buffer_info
        );

        _color_views[i]->create(
            _color_buffers[i]->native(),
            _color_buffers[i]->format(),
            vk::ImageViewType::e2D,
            vk::ImageAspectFlagBits::eColor
        );
    }
}

// =============================================================================
void vkColorDepthPass::_create_depth_buffer() {
    vkImage::ImageInfo const depth_stencil_info {
        .type         = vk::ImageType::e2D,
        .samples      = _msaa_samples,
        .usage_flags  = vk::ImageUsageFlagBits::eDepthStencilAttachment,
        .memory_flags = vk::MemoryPropertyFlagBits::eDeviceLocal,
    };

    BTX_TRACE("Creating depth buffer for color/depth pass");
    _depth_buffer->create(
        vk::Extent2D {
            .width = Renderer::swapchain().size().width,
            .height = Renderer::swapchain().size().height
        },
        _depth_format,
        depth_stencil_info
    );

    _depth_view->create(
        _depth_buffer->native(),
        _depth_buffer->format(),
        vk::ImageViewType::e2D,
        vk::ImageAspectFlagBits::eDepth
    );
}

// =============================================================================
void vkColorDepthPass::_destroy_color_buffers() {
    for(auto &view : _color_views) {
        view->destroy();
    }

    for(auto &buffer : _color_buffers) {
        buffer->destroy();
    }
}

// =============================================================================
void vkColorDepthPass::_destroy_depth_buffer() {
    _depth_view->destroy();
    _depth_buffer->destroy();
}

} // namespace btx