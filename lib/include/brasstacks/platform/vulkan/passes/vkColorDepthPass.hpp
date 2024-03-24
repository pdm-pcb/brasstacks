#ifndef BRASSTACKS_PLATFORM_VULKAN_PASSES_VKCOLORPASS_HPP
#define BRASSTACKS_PLATFORM_VULKAN_PASSES_VKCOLORPASS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/platform/vulkan/passes/vkRenderPassBase.hpp"

namespace btx {

class Renderer;

class vkImage;
class vkImageView;

class vkColorDepthPass final : public vkRenderPassBase {
public:

    vkColorDepthPass(Renderer const &renderer, bool const present);
    ~vkColorDepthPass() override;

    void destroy_swapchain_resources();
    void recreate_swapchain_resources();

    auto const & color_views() const { return _color_views; }
    auto const & depth_views() const { return _depth_views; }
    auto msaa_samples() const { return _msaa_samples; }

    vkColorDepthPass() = delete;

    vkColorDepthPass(vkColorDepthPass &&) = delete;
    vkColorDepthPass(vkColorDepthPass const &) = delete;

    vkColorDepthPass & operator=(vkColorDepthPass &&) = delete;
    vkColorDepthPass & operator=(vkColorDepthPass const &) = delete;

private:
    vk::Format _depth_format;

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
    void _init_attachments(bool const present);
    void _init_subpasses();

    void _create_color_buffers();
    void _create_depth_buffers();
    void _destroy_color_buffers();
    void _destroy_depth_buffers();
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_PASSES_VKCOLORPASS_HPP