#include "brasstacks/platform/vulkan/resources/buffers/vkBuffer.hpp"

namespace btx {

// =============================================================================
vkBuffer::vkBuffer(vkDevice const &device) :
    _device { device }
{

}

// =============================================================================
vkBuffer::~vkBuffer() {

}

} // namespace btx