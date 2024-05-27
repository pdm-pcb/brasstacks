#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/resources/vkSwapchainImage.hpp"

#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"

namespace btx {

// =============================================================================
vkSwapchainImage::vkSwapchainImage() :
    _handle { nullptr },
    _format { vk::Format::eUndefined }
{ }

// =============================================================================
vkSwapchainImage::vkSwapchainImage(vkSwapchainImage &&rhs) :
    _handle { rhs._handle },
    _format { rhs._format }
{
    rhs._handle = nullptr;
    rhs._format = vk::Format::eUndefined;
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

    vk::ImageMemoryBarrier2KHR barrier {
        .pNext = nullptr,
        .srcStageMask  = vk::PipelineStageFlagBits2KHR::eAllCommands,
        .srcAccessMask = vk::AccessFlagBits2KHR::eMemoryWrite,
        .dstStageMask  = vk::PipelineStageFlagBits2KHR::eAllCommands,
        .dstAccessMask = vk::AccessFlagBits2KHR::eMemoryRead
                         | vk::AccessFlagBits2KHR::eMemoryWrite,
        .oldLayout = old_layout,
        .newLayout = new_layout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = _handle,
        .subresourceRange {
            .aspectMask     = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel   = 0u,
            .levelCount     = VK_REMAINING_MIP_LEVELS,
            .baseArrayLayer = 0u,
            .layerCount     = VK_REMAINING_ARRAY_LAYERS,
        }
    };

    if(old_layout == vk::ImageLayout::eUndefined) {
        if(new_layout == vk::ImageLayout::eColorAttachmentOptimal) {
            barrier.srcAccessMask = vk::AccessFlagBits2KHR::eColorAttachmentRead
                                    | vk::AccessFlagBits2KHR::eColorAttachmentWrite;
            barrier.dstAccessMask = vk::AccessFlagBits2KHR::eColorAttachmentRead
                                    | vk::AccessFlagBits2KHR::eColorAttachmentWrite;
        }
    }
    else if(old_layout == vk::ImageLayout::eColorAttachmentOptimal) {
        if(new_layout == vk::ImageLayout::ePresentSrcKHR) {
            barrier.srcAccessMask = vk::AccessFlagBits2KHR::eColorAttachmentRead
                                    | vk::AccessFlagBits2KHR::eColorAttachmentWrite;
            barrier.dstAccessMask = { };
        }
    }

    auto dep_info = vk::DependencyInfoKHR {
        .pNext = nullptr,
        .dependencyFlags = { },
        .memoryBarrierCount = { },
        .pMemoryBarriers = { },
        .bufferMemoryBarrierCount = { },
        .pBufferMemoryBarriers = { },
        .imageMemoryBarrierCount = 1u,
        .pImageMemoryBarriers = &barrier,
    };

    cmd_buffer.native().pipelineBarrier2KHR(dep_info);
}

} // namespace btx