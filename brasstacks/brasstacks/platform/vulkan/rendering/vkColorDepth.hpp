#ifndef BRASSTACKS_PLATFORM_VULKAN_PASSES_VKCOLORDEPTH_HPP
#define BRASSTACKS_PLATFORM_VULKAN_PASSES_VKCOLORDEPTH_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkColorDepth final {
public:
    vkColorDepth();
    ~vkColorDepth() = default;

    void destroy_swapchain_resources();
    void create_swapchain_resources();

    void begin();
    void end();

    auto const & depth_view() const { return _depth_view; }
    auto depth_format() const { return _depth_format; }

    vkColorDepth(vkColorDepth &&) = delete;
    vkColorDepth(vkColorDepth const &) = delete;

    vkColorDepth & operator=(vkColorDepth &&) = delete;
    vkColorDepth & operator=(vkColorDepth const &) = delete;

private:
    vk::Format  _depth_format;
    vkImage     _depth_buffer;
    vkImageView _depth_view;

    std::vector<vk::RenderingAttachmentInfo> _color_attachments;
    vk::RenderingAttachmentInfo              _depth_attachment;

    void _find_depth_stencil_format();
    void _create_depth_buffer();

    void _init_attachment_info();

    void _destroy_depth_buffer();
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_PASSES_VKCOLORDEPTH_HPP