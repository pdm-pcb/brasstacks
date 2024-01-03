#ifndef BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKUILAYERPASS_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKUILAYERPASS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/platform/vulkan/rendering/vkRenderPass.hpp"

namespace btx {

class vkPhysicalDevice;
class vkDevice;
class vkImage;

class vkUILayerPass final : public vkRenderPass {
public:
    vkUILayerPass(vkDevice const &device, vk::Format const format);

    ~vkUILayerPass() override = default;

    vkUILayerPass() = delete;

    vkUILayerPass(vkUILayerPass &&) = delete;
    vkUILayerPass(vkUILayerPass const &) = delete;

    vkUILayerPass & operator=(vkUILayerPass &&) = delete;
    vkUILayerPass & operator=(vkUILayerPass const &) = delete;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKUILAYERPASS_HPP