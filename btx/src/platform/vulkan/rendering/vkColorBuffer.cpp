#include "brasstacks/platform/vulkan/rendering/vkColorBuffer.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/vkSwapchain.hpp"
#include "brasstacks/platform/vulkan/resources/vkImage.hpp"

namespace btx {

// =============================================================================
vkColorBuffer::vkColorBuffer(vkDevice const &device,
                             vkSwapchain const &swapchain)
{
    // _image = new vkImage(
    //     device.native(),
    //     swapchain.render_area().extent,
    //     swapchain.image_format(),
    //     vk::SampleCountFlagBits::e1,
    //     (
    //         vk::ImageUsageFlagBits::eColorAttachment |
    //         vk::ImageUsageFlagBits::eTransientAttachment
    //     ),
    //     vk::MemoryPropertyFlagBits::eDeviceLocal
    // );

    // _image->create_view(vk::ImageViewType::e2D);
}

vkColorBuffer::~vkColorBuffer() {
    delete _image;
}

} // namespace btx