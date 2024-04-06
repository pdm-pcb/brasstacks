#ifndef BRASSTACKS_PLATFORM_VULKAN_PASSES_VKCOLORDEPTHPASS_HPP
#define BRASSTACKS_PLATFORM_VULKAN_PASSES_VKCOLORDEPTHPASS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/platform/vulkan/passes/vkRenderPassBase.hpp"
#include "brasstacks/platform/vulkan/resources/vkImage.hpp"
#include "brasstacks/platform/vulkan/resources/vkImageView.hpp"

namespace btx {

class vkFramebuffer;

class vkColorDepthPass final : public vkRenderPassBase {
public:
    vkColorDepthPass();
    ~vkColorDepthPass() override = default;

    void create();
    void destroy();

    void destroy_swapchain_resources();
    void create_swapchain_resources();

    void begin(vkFramebuffer const &framebuffer);
    void end();

    auto const & color_views() const { return _color_views; }
    auto const & depth_view() const { return *_depth_view; }
    auto msaa_samples() const { return _msaa_samples; }
    auto color_format() const { return _color_format; }
    auto depth_format() const { return _depth_format; }

    vkColorDepthPass(vkColorDepthPass &&) = delete;
    vkColorDepthPass(vkColorDepthPass const &) = delete;

    vkColorDepthPass & operator=(vkColorDepthPass &&) = delete;
    vkColorDepthPass & operator=(vkColorDepthPass const &) = delete;

private:
    vk::Format _color_format;
    vk::Format _depth_format;

    vk::SampleCountFlagBits _msaa_samples;

    std::vector<std::unique_ptr<vkImage>>     _color_buffers;
    std::vector<std::unique_ptr<vkImageView>> _color_views;

    std::unique_ptr<vkImage>     _depth_buffer;
    std::unique_ptr<vkImageView> _depth_view;

    std::vector<vk::AttachmentDescription> _attachment_descriptions;
    std::vector<vk::AttachmentReference>   _color_attachments;
    vk::AttachmentReference                _depth_attachment;
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

#endif // BRASSTACKS_PLATFORM_VULKAN_PASSES_VKCOLORDEPTHPASS_HPP