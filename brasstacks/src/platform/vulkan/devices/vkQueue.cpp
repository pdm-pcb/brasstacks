#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/devices/vkQueue.hpp"

namespace btx {

// =============================================================================
vkQueue::vkQueue() :
    _handle       { nullptr },
    _family_index { std::numeric_limits<uint32_t>::max() }
{ }

// =============================================================================
void vkQueue::set_family_index(uint32_t const index) {
    if(_handle) {
        BTX_CRITICAL("Device queue {} already retrieved", _handle);
        return;
    }

    _family_index = index;
    _handle = Renderer::device().native().getQueue(_family_index, 0u);

    if(!_handle) {
        BTX_CRITICAL("Failed to retrieve device queue.");
        return;
    }

    BTX_TRACE("Received queue {} from device.", _handle);
}

// =============================================================================
void vkQueue::clear_family_index() {
    BTX_TRACE("Clearing family index {} for queue {}.", _family_index, _handle);
    _handle = nullptr;
    _family_index = std::numeric_limits<uint32_t>::max();
}

} // namespace btx