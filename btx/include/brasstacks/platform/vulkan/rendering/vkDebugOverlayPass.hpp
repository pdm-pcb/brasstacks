#ifndef BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKDEBUGOVERLAYPASS_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKDEBUGOVERLAYPASS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/platform/vulkan/rendering/vkRenderPass.hpp"

namespace btx {

class vkPhysicalDevice;
class vkDevice;
class vkImage;

class vkDebugOverlayPass final : public vkRenderPass {
public:
    vkDebugOverlayPass(vkDevice const &device, vk::Format const format);

    ~vkDebugOverlayPass() override = default;

    vkDebugOverlayPass() = delete;

    vkDebugOverlayPass(vkDebugOverlayPass &&) = delete;
    vkDebugOverlayPass(vkDebugOverlayPass const &) = delete;

    vkDebugOverlayPass & operator=(vkDebugOverlayPass &&) = delete;
    vkDebugOverlayPass & operator=(vkDebugOverlayPass const &) = delete;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKDEBUGOVERLAYPASS_HPP