#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/rendering/vkRenderPass.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"

namespace btx {

// =============================================================================
vkRenderPass::vkRenderPass(Renderer const &renderer) :
    _renderer { renderer }
{ }

// =============================================================================
void vkRenderPass::_create(vk::RenderPassCreateInfo const &create_info) {
    auto const result = device().native().createRenderPass(create_info);
    if(result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to create render pass: '{}'",
                     vk::to_string(result.result));
        return;
    }

    _handle = result.value;
    BTX_TRACE("Created render pass {}", _handle);
}

// =============================================================================
vkRenderPass::~vkRenderPass() {
    BTX_TRACE("Destroying render pass {}", _handle);
    device().native().destroyRenderPass(_handle);
}

} // namespace btx