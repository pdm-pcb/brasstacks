#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/devices/vkQueue.hpp"

namespace btx {

// =============================================================================
vkQueue::vkQueue() :
    _handle       { nullptr },
    _family_index { std::numeric_limits<uint32_t>::max() }
{ }

// =============================================================================
void vkQueue::set(uint32_t const family_index) {
    if(_handle || _family_index != std::numeric_limits<uint32_t>::max()) {
        BTX_CRITICAL("Queue {}, index {} already exists",
                     _handle,
                     _family_index);
        return;
    }

    _family_index = family_index;
    _handle = Renderer::device().native().getQueue(_family_index, 0u);

    if(!_handle) {
        BTX_CRITICAL("Failed to get device queue, index {}.", _family_index);
        return;
    }

    BTX_TRACE("Created queue {}, index {}.", _handle, _family_index);
}

// =============================================================================
void vkQueue::clear() {
    BTX_TRACE("Clearing queue {}, index {}", _handle, _family_index);
    _handle = nullptr;
    _family_index = std::numeric_limits<uint32_t>::max();
}

} // namespace btx