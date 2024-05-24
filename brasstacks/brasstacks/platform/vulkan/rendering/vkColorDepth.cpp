#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/rendering/vkColorDepth.hpp"

#include "brasstacks/platform/vulkan/vkInstance.hpp"
#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"
#include "brasstacks/platform/vulkan/swapchain/vkSwapchain.hpp"
#include "brasstacks/platform/vulkan/resources/vkSwapchainImage.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkPipeline.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorPool.hpp"

namespace btx {

vkColorDepth::vkColorDepth() :
    _depth_format       { vk::Format::eUndefined },
    _depth_buffer       { },
    _depth_view         { },
    _color_attachments  { },
    _depth_attachment   { }
{
    _find_depth_stencil_format();
}

// =============================================================================
void vkColorDepth::destroy_swapchain_resources() {
    _destroy_depth_buffer();
}

// =============================================================================
void vkColorDepth::create_swapchain_resources() {
    _create_depth_buffer();
    _init_attachment_info();
}

// =============================================================================
void vkColorDepth::begin() {
    auto &color_buffer = *Renderer::swapchain().images()[Renderer::image_index()];
    color_buffer.transition_layout(Renderer::cmd_buffer(),
                                   vk::ImageLayout::eUndefined,
                                   vk::ImageLayout::eColorAttachmentOptimal);

    _depth_buffer.transition_layout(Renderer::cmd_buffer(),
                                    vk::ImageLayout::eUndefined,
                                    vk::ImageLayout::eDepthAttachmentOptimal);

    vk::Rect2D const render_area = {
        .offset { .x = 0u, .y = 0u },
        .extent {
            .width  = Renderer::swapchain().size().width,
            .height = Renderer::swapchain().size().height,
        },
    };

    Renderer::cmd_buffer().begin_rendering(
        vk::RenderingInfoKHR {
            .pNext                = nullptr,
            .flags                = { },
            .renderArea           = render_area,
            .layerCount           = 1u,
            .viewMask             = 0u,
            .colorAttachmentCount = 1u,
            .pColorAttachments    = &_color_attachments[Renderer::image_index()],
            .pDepthAttachment     = &_depth_attachment,
            .pStencilAttachment   = nullptr,
        }
    );
}

// =============================================================================
void vkColorDepth::end() {
    Renderer::cmd_buffer().end_rendering();

    auto &color_buffer = *Renderer::swapchain().images()[Renderer::image_index()];
    color_buffer.transition_layout(Renderer::cmd_buffer(),
                                   vk::ImageLayout::eColorAttachmentOptimal,
                                   vk::ImageLayout::ePresentSrcKHR);
}

// =============================================================================
void vkColorDepth::_find_depth_stencil_format() {
    static std::array<vk::Format, 2> const depth_formats {
        vk::Format::eD32Sfloat,
        vk::Format::eD16Unorm,
    };

    auto const physical_device = RenderConfig::current_device->device->native();
    for(auto const format : depth_formats) {
        auto props = physical_device.getFormatProperties(format);
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
void vkColorDepth::_create_depth_buffer() {
    BTX_TRACE("Creating depth buffer for color/depth pass");
    _depth_buffer.create(
        vk::Extent2D {
            .width = Renderer::swapchain().size().width,
            .height = Renderer::swapchain().size().height
        },
        _depth_format,
        vkImage::ImageInfo {
            .type         = vk::ImageType::e2D,
            .samples      = vk::SampleCountFlagBits::e1,
            .usage_flags  = vk::ImageUsageFlagBits::eDepthStencilAttachment,
            .memory_flags = vk::MemoryPropertyFlagBits::eDeviceLocal,
            .aspect_flags = vk::ImageAspectFlagBits::eDepth,
        }
    );

    _depth_view.create(
        _depth_buffer.native(),
        _depth_buffer.format(),
        vk::ImageViewType::e2D,
        vk::ImageAspectFlagBits::eDepth
    );
}

// =============================================================================
void vkColorDepth::_init_attachment_info() {
    if(_color_attachments.size() != Renderer::swapchain().images().size()) {
        _color_attachments.resize(Renderer::swapchain().images().size());
    }

    constexpr std::array<vk::ClearValue, 2> clear_values = {{
        { .color { std::array<float, 4> {{ 0.08f, 0.08f, 0.16f, 1.0f }} }},
        { .depthStencil { .depth = 1.0f, .stencil = 1u } }
    }};

    for(uint32_t i = 0; i < _color_attachments.size(); ++i) {
        auto &color = _color_attachments[i];
        auto &view = Renderer::swapchain().image_views()[i]->native();

        color.pNext              = nullptr;
        color.imageView          = view;
        color.imageLayout        = vk::ImageLayout::eColorAttachmentOptimal;
        color.resolveMode        = vk::ResolveModeFlagBits::eNone;
        color.resolveImageView   = nullptr;
        color.resolveImageLayout = vk::ImageLayout::eUndefined;
        color.loadOp             = vk::AttachmentLoadOp::eClear;
        color.storeOp            = vk::AttachmentStoreOp::eStore;
        color.clearValue         = clear_values[0];
    }

    _depth_attachment.pNext              = nullptr;
    _depth_attachment.imageView          = _depth_view.native();
    _depth_attachment.imageLayout =
        vk::ImageLayout::eDepthAttachmentOptimal;
    _depth_attachment.resolveMode        = vk::ResolveModeFlagBits::eNone;
    _depth_attachment.resolveImageView   = nullptr;
    _depth_attachment.resolveImageLayout = vk::ImageLayout::eUndefined;
    _depth_attachment.loadOp             = vk::AttachmentLoadOp::eClear;
    _depth_attachment.storeOp            = vk::AttachmentStoreOp::eStore;
    _depth_attachment.clearValue         = clear_values[1];
}

// =============================================================================
void vkColorDepth::_destroy_depth_buffer() {
    _depth_view.destroy();
    _depth_buffer.destroy();
}

} // namespace btx