#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/resources/vkSwapchainImage.hpp"

#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"

namespace btx {

// =============================================================================
vkSwapchainImage::vkSwapchainImage() :
    _handle { nullptr },
    _format { vk::Format::eUndefined },
    _layout { vk::ImageLayout::eUndefined }
{ }

// =============================================================================
vkSwapchainImage::vkSwapchainImage(vkSwapchainImage &&rhs) :
    _handle { rhs._handle },
    _format { rhs._format },
    _layout { rhs._layout }
{
    rhs._handle = nullptr;
    rhs._format = vk::Format::eUndefined;
    rhs._layout = vk::ImageLayout::eUndefined;
}

// =============================================================================
void vkSwapchainImage::create(vk::Image const &handle, vk::Format const format)
{
    if(_handle) {
        BTX_CRITICAL("Swapchain image {} already exists", _handle);
    }

    _handle = handle;
    _format = format;
}

// =============================================================================
void vkSwapchainImage::destroy() {
    _handle = nullptr;
    _format = vk::Format::eUndefined;
    _layout = vk::ImageLayout::eUndefined;
}

// =============================================================================
void vkSwapchainImage::transition_layout(vkCmdBuffer const &cmd_buffer,
                                         vk::ImageLayout const old_layout,
                                         vk::ImageLayout const new_layout)
{
    // BTX_TRACE("Swapchain image {}: '{:s}'->'{:s}'",
    //           _handle,
    //           vk::to_string(old_layout),
    //           vk::to_string(new_layout));

    vk::ImageMemoryBarrier barrier {
        .srcAccessMask = { },
        .dstAccessMask = { },
        .oldLayout = old_layout,
        .newLayout = new_layout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = _handle,
        .subresourceRange {
            .aspectMask     = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel   = 0u,
            .levelCount     = 1u,
            .baseArrayLayer = 0u,
            .layerCount     = 1u,
        }
    };

    vk::PipelineStageFlags src_stage = vk::PipelineStageFlagBits::eNone;
    vk::PipelineStageFlags dst_stage = vk::PipelineStageFlagBits::eNone;

    if(old_layout == vk::ImageLayout::eUndefined) {
        barrier.srcAccessMask = vk::AccessFlagBits::eNone;

        if(new_layout == vk::ImageLayout::eColorAttachmentOptimal) {
            barrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead
                                    | vk::AccessFlagBits::eColorAttachmentWrite;

            src_stage = vk::PipelineStageFlagBits::eTopOfPipe;
            dst_stage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        }
        else if(new_layout == vk::ImageLayout::eDepthAttachmentOptimal) {
            barrier.dstAccessMask =
                vk::AccessFlagBits::eDepthStencilAttachmentRead
                | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

            src_stage = vk::PipelineStageFlagBits::eTopOfPipe;
            dst_stage = vk::PipelineStageFlagBits::eEarlyFragmentTests
                        | vk::PipelineStageFlagBits::eLateFragmentTests;
        }
        else {
            BTX_CRITICAL("Unsupported image layout transition");
            return;
        }
    }
    else if(old_layout == vk::ImageLayout::eColorAttachmentOptimal) {
        barrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentRead
                                | vk::AccessFlagBits::eColorAttachmentWrite;

        barrier.dstAccessMask = vk::AccessFlagBits::eNone;

        src_stage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        dst_stage = vk::PipelineStageFlagBits::eBottomOfPipe;
    }
    else {
        BTX_CRITICAL("Unsupported image layout transition");
        return;
    }

    cmd_buffer.native().pipelineBarrier(
        src_stage,  // Source stage
        dst_stage,  // Destination stage
        { },        // Dependency flags
        nullptr,    // Memory barriers
        nullptr,    // Buffer memory barriers
        { barrier } // Image memory barriers
    );

    _layout = barrier.newLayout;
}

} // namespace btx