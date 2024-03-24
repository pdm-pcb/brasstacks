#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/passes/vkRenderPassBase.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"

namespace btx {

// =============================================================================
vkRenderPassBase::vkRenderPassBase(Renderer const &renderer) :
    _renderer { renderer }
{ }

// =============================================================================
void vkRenderPassBase::_create(vk::RenderPassCreateInfo const &create_info) {
    _handle = device().native().createRenderPass(create_info);
    BTX_TRACE("Created render pass {}", _handle);
}

// =============================================================================
vkRenderPassBase::~vkRenderPassBase() {
    BTX_TRACE("Destroying render pass {}", _handle);
    device().native().destroyRenderPass(_handle);
}

} // namespace btx