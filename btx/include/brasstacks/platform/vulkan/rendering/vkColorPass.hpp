#ifndef BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKCOLORPASS_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKCOLORPASS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/platform/vulkan/rendering/vkRenderPass.hpp"

namespace btx {

class vkPhysicalDevice;
class vkDevice;
class vkImage;

class vkColorPass final : public vkRenderPass {
public:
    vkColorPass(vkDevice const &device, vk::Format const format,
                vk::SampleCountFlagBits const msaa_samples);

    ~vkColorPass() override = default;

    vkColorPass() = delete;

    vkColorPass(vkColorPass &&) = delete;
    vkColorPass(vkColorPass const &) = delete;

    vkColorPass & operator=(vkColorPass &&) = delete;
    vkColorPass & operator=(vkColorPass const &) = delete;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKCOLORPASS_HPP