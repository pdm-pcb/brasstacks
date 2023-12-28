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

    auto const & color_view() const { return *_color_view; }
    auto const & depth_view() const { return *_depth_view; }

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

    vkImage *_color_buffer;
    vkImageView *_color_view;

    vkImage *_depth_buffer;
    vkImageView *_depth_view;

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