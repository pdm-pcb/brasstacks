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
                     vk::Extent2D const &extent,
                     vk::SampleCountFlagBits const msaa_samples);

    ~vkColorDepthPass() override;

    auto const & color_views() const { return _color_views; }
    auto const & depth_views() const { return _depth_views; }

    vkColorDepthPass() = delete;

    vkColorDepthPass(vkColorDepthPass &&) = delete;
    vkColorDepthPass(vkColorDepthPass const &) = delete;

    vkColorDepthPass & operator=(vkColorDepthPass &&) = delete;
    vkColorDepthPass & operator=(vkColorDepthPass const &) = delete;

private:
    vk::Extent2D _extent;
    vk::SampleCountFlagBits _msaa_samples;
    vk::Format _color_format;
    vk::Format _depth_format;

    std::vector<vkImage *>     _color_buffers;
    std::vector<vkImageView *> _color_views;
    std::vector<vkImage *>     _depth_buffers;
    std::vector<vkImageView *> _depth_views;

    std::vector<vk::AttachmentDescription> _attachment_descriptions;
    std::vector<vk::AttachmentReference>   _color_attachments;
    vk::AttachmentReference                _depth_attachment;
    std::vector<vk::AttachmentReference>   _resolve_attachments;
    std::vector<vk::SubpassDescription>    _subpasses;
    std::vector<vk::SubpassDependency>     _subpass_dependencies;

    void _find_depth_stencil_format(vkPhysicalDevice const &physical_device);
    void _create_color_buffer();
    void _create_depth_buffer();
    void _init_attachments();
    void _init_subpasses();
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKCOLORDEPTHPASS_HPP