#ifndef BRASSTACKS_PLATFORM_VULKAN_PASSES_VKCOLORPASS_HPP
#define BRASSTACKS_PLATFORM_VULKAN_PASSES_VKCOLORPASS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/platform/vulkan/passes/vkRenderPassBase.hpp"

namespace btx {

class vkImage;
class vkImageView;

class vkColorDepthPass final : public vkRenderPassBase {
public:
    vkColorDepthPass();
    ~vkColorDepthPass() override = default;

    void destroy_swapchain_resources();
    void create_swapchain_resources();

    auto const & color_views() const { return _color_views; }
    auto const & depth_view() const { return *_depth_view; }
    auto msaa_samples() const { return _msaa_samples; }
    auto depth_format() const { return _depth_format; }

    vkColorDepthPass(vkColorDepthPass &&) = delete;
    vkColorDepthPass(vkColorDepthPass const &) = delete;

    vkColorDepthPass & operator=(vkColorDepthPass &&) = delete;
    vkColorDepthPass & operator=(vkColorDepthPass const &) = delete;

private:
    vk::Format _depth_format;

    vk::SampleCountFlagBits _msaa_samples;

    std::vector<vkImage *>     _color_buffers;
    std::vector<vkImageView *> _color_views;

    vkImage     *_depth_buffer;
    vkImageView *_depth_view;

    std::vector<vk::AttachmentDescription> _attachment_descriptions;
    std::vector<vk::AttachmentReference>   _color_attachments;
    vk::AttachmentReference                _depth_attachment;
    std::vector<vk::AttachmentReference>   _resolve_attachments;
    std::vector<vk::SubpassDescription>    _subpasses;
    std::vector<vk::SubpassDependency>     _subpass_dependencies;

    void _find_depth_stencil_format();
    void _init_attachment_details();
    void _init_subpasses();

    void _create_color_buffers();
    void _create_depth_buffer();
    void _destroy_color_buffers();
    void _destroy_depth_buffer();
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_PASSES_VKCOLORPASS_HPP