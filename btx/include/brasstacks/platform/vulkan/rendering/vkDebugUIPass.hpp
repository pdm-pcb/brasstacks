#ifndef BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKDEBUGUIPASS_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKDEBUGUIPASS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/platform/vulkan/rendering/vkRenderPass.hpp"

namespace btx {

class vkDevice;
class vkImage;
class vkImageView;

class vkDebugUIPass final : public vkRenderPass {
public:
    vkDebugUIPass(vkDevice const &device, vk::Format const format,
                  vk::Extent2D const &extent);

    ~vkDebugUIPass() override;

    auto const & color_views() const { return _color_views; }

    vkDebugUIPass() = delete;

    vkDebugUIPass(vkDebugUIPass &&) = delete;
    vkDebugUIPass(vkDebugUIPass const &) = delete;

    vkDebugUIPass & operator=(vkDebugUIPass &&) = delete;
    vkDebugUIPass & operator=(vkDebugUIPass const &) = delete;

private:
    vk::Extent2D _extent;
    vk::Format _color_format;

    std::vector<vkImage *>     _color_buffers;
    std::vector<vkImageView *> _color_views;

    std::vector<vk::AttachmentDescription> _attachment_descriptions;
    std::vector<vk::AttachmentReference>   _color_attachments;
    std::vector<vk::SubpassDescription>    _subpasses;
    std::vector<vk::SubpassDependency>     _subpass_dependencies;

    void _create_color_buffer();
    void _init_attachments();
    void _init_subpasses();
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKDEBUGUIPASS_HPP