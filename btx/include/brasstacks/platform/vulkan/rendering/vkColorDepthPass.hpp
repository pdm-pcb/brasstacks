#ifndef BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKCOLORDEPTHPASS_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKCOLORDEPTHPASS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/platform/vulkan/rendering/vkRenderPass.hpp"

namespace btx {

class vkPhysicalDevice;
class vkDevice;
class vkImage;
class vkImageView;

class vkColorDepthPass final : public vkRenderPass {
public:
    vkColorDepthPass(vkPhysicalDevice const &physical_device,
                     vkDevice const &device, vk::Format const format,
                     vk::SampleCountFlagBits const msaa_samples);

    ~vkColorDepthPass() override = default;

    vkColorDepthPass() = delete;

    vkColorDepthPass(vkColorDepthPass &&) = delete;
    vkColorDepthPass(vkColorDepthPass const &) = delete;

    vkColorDepthPass & operator=(vkColorDepthPass &&) = delete;
    vkColorDepthPass & operator=(vkColorDepthPass const &) = delete;

private:
    vk::Format _depth_format;

    vkImage *_color_buffer;
    vkImageView *_color_view;

    vkImage *_depth_buffer;

    void _find_depth_stencil_format(vkPhysicalDevice const &physical_device);
    void _init_color_buffer(vk::Format const format, vk::Extent2D const extent,
                            vk::SampleCountFlagBits const samples);
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKCOLORDEPTHPASS_HPP