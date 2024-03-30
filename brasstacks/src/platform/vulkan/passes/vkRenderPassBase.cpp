#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/passes/vkRenderPassBase.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"

namespace btx {

// =============================================================================
vkRenderPassBase::vkRenderPassBase() :
    _handle { nullptr }
{ }

// =============================================================================
void vkRenderPassBase::_create(vk::RenderPassCreateInfo const &create_info) {
    if(_handle != nullptr) {
        BTX_CRITICAL("Render pass {} already exists", _handle);
    }
    _handle = Renderer::device().native().createRenderPass(create_info);
    BTX_TRACE("Created render pass {}", _handle);
}

// =============================================================================
void vkRenderPassBase::_destroy() {
    BTX_TRACE("Destroying render pass {}", _handle);
    Renderer::device().native().destroyRenderPass(_handle);
    _handle = nullptr;
}

} // namespace btx