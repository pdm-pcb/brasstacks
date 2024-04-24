#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/rendering/vkColorDepth.hpp"

#include "brasstacks/platform/vulkan/vkInstance.hpp"
#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"
#include "brasstacks/platform/vulkan/swapchain/vkSwapchain.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkPipeline.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorPool.hpp"

#include "brasstacks/platform/imgui/UIOverlay.hpp"

namespace btx {

vkColorDepth::vkColorDepth() :
    _depth_format       { vk::Format::eUndefined },
    _color_buffers      { },
    _color_views        { },
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
    _destroy_color_buffers();
}

// =============================================================================
void vkColorDepth::create_swapchain_resources() {
    _create_color_buffers();
    _create_depth_buffer();
    _init_attachment_info();
}

// =============================================================================
void vkColorDepth::begin() {
    auto &color_buffer = _color_buffers[Renderer::image_index()];
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
        vk::RenderingInfoKHR  {
            .pNext                = nullptr,
            .flags                = { },
            .renderArea           = render_area,
            .layerCount           = 1u,
            .viewMask             = 0u,
            .colorAttachmentCount = static_cast<uint32_t>(
                _color_attachments.size()
            ),
            .pColorAttachments    = _color_attachments.data(),
            .pDepthAttachment     = &_depth_attachment,
            .pStencilAttachment   = nullptr,
        }
    );
}

// =============================================================================
void vkColorDepth::end() {
    Renderer::cmd_buffer().end_rendering();

    auto &color_buffer = _color_buffers[Renderer::image_index()];
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

    auto const physical_device = RenderConfig::current_device->handle;
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
void vkColorDepth::_create_color_buffers() {
    auto const image_count = Renderer::swapchain().images().size();
    BTX_TRACE("Creating {} color buffers for color/depth pass", image_count);

    if(_color_buffers.size() != image_count
       || _color_views.size() != image_count)
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
                return vkImage { };
            }
        );

        // And likewise the views
        std::generate_n(
            std::back_inserter(_color_views),
            _color_views.capacity(),
            []() {
                return vkImageView { };
            }
        );
    }

    for(size_t i = 0; i < image_count; ++i) {
        _color_buffers[i].create(
            vk::Extent2D {
                .width = Renderer::swapchain().size().width,
                .height = Renderer::swapchain().size().height
            },
            Renderer::swapchain().image_format(),
            vkImage::ImageInfo {
                .type         = vk::ImageType::e2D,
                .samples      = vk::SampleCountFlagBits::e1,
                .usage_flags  = (vk::ImageUsageFlagBits::eColorAttachment |
                                 vk::ImageUsageFlagBits::eTransientAttachment),
                .memory_flags = vk::MemoryPropertyFlagBits::eDeviceLocal,
                .aspect_flags = vk::ImageAspectFlagBits::eColor,
            }
        );

        _color_views[i].create(
            _color_buffers[i].native(),
            _color_buffers[i].format(),
            vk::ImageViewType::e2D,
            vk::ImageAspectFlagBits::eColor
        );
    }
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
    if(_color_attachments.size() != _color_buffers.size()) {
        _color_attachments.resize(_color_buffers.size());
    }

    constexpr std::array<vk::ClearValue, 2> clear_values = {{
        { .color { std::array<float, 4> {{ 0.08f, 0.08f, 0.16f, 1.0f }} }},
        { .depthStencil { .depth = 1.0f, .stencil = 1u } }
    }};

    for(uint32_t i = 0; i < _color_attachments.size(); ++i) {
        auto &color = _color_attachments[i];

        color.pNext              = nullptr;
        color.imageView          = _color_views[i].native();
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
void vkColorDepth::_destroy_color_buffers() {
    for(auto &view : _color_views) {
        view.destroy();
    }

    for(auto &buffer : _color_buffers) {
        buffer.destroy();
    }
}

// =============================================================================
void vkColorDepth::_destroy_depth_buffer() {
    _depth_view.destroy();
    _depth_buffer.destroy();
}

} // namespace btx