#ifndef BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKCOLORPASS_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKCOLORPASS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/platform/vulkan/rendering/vkRenderPass.hpp"

namespace btx {

class vkDevice;
class vkSwapchain;
class vkImage;
class vkImageView;

class vkColorPass final : public vkRenderPass {
public:

    vkColorPass(vkDevice const &device, vkSwapchain const &swapchain,
                bool const present);

    ~vkColorPass() override;

    auto const & color_views() const { return _color_views; }
    auto const & depth_views() const { return _depth_views; }
    auto msaa_samples() const { return _msaa_samples; }

    vkColorPass() = delete;

    vkColorPass(vkColorPass &&) = delete;
    vkColorPass(vkColorPass const &) = delete;

    vkColorPass & operator=(vkColorPass &&) = delete;
    vkColorPass & operator=(vkColorPass const &) = delete;

private:
    vk::Extent2D _extent;
    vk::Format   _color_format;
    vk::Format   _depth_format;

    vk::SampleCountFlagBits _msaa_samples;

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

    void _find_depth_stencil_format();
    void _create_color_buffer();
    void _create_depth_buffer();
    void _init_attachments(bool const present);
    void _init_subpasses();
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKCOLORPASSPRESENT_HPP