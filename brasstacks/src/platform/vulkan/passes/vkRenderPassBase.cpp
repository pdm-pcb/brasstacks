#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/passes/vkRenderPassBase.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"

namespace btx {

// =============================================================================
void vkRenderPassBase::_create(vk::RenderPassCreateInfo const &create_info) {
    _handle = Renderer::device().native().createRenderPass(create_info);
    BTX_TRACE("Created render pass {}", _handle);
}

// =============================================================================
vkRenderPassBase::~vkRenderPassBase() {
    BTX_TRACE("Destroying render pass {}", _handle);
    Renderer::device().native().destroyRenderPass(_handle);
}

} // namespace btx