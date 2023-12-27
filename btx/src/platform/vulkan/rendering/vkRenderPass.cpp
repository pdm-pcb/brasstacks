#include "brasstacks/core.hpp"
#include "brasstacks/platform/vulkan/rendering/vkRenderPass.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"

namespace btx {

// =============================================================================
vkRenderPass::vkRenderPass(vkDevice const &device) :
    _device { device }
{ }

// =============================================================================
void vkRenderPass::create(vk::RenderPassCreateInfo const &create_info) {
    auto const result = _device.native().createRenderPass(create_info);
    if(result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to create render pass: '{}'",
                     vk::to_string(result.result));
        return;
    }

    _handle = result.value;
}

// =============================================================================
vkRenderPass::~vkRenderPass() {
    _device.native().destroyRenderPass(_handle);
}

} // namespace btx