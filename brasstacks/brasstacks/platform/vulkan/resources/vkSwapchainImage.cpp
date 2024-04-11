#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/resources/vkSwapchainImage.hpp"

namespace btx {

// =============================================================================
vkSwapchainImage::vkSwapchainImage() :
    _handle { nullptr },
    _format { vk::Format::eUndefined }
{ }

// =============================================================================
vkSwapchainImage::vkSwapchainImage(vkSwapchainImage &&rhs) :
    _handle { rhs._handle },
    _format { rhs._format }
{
    rhs._handle = nullptr;
    rhs._format = vk::Format::eUndefined;
}

// =============================================================================
void vkSwapchainImage::create(vk::Image const &handle, vk::Format const format) {
    if(_handle) {
        BTX_CRITICAL("Swapchain image {} already exists", _handle);
    }

    _handle = handle;
    _format = format;
}

} // namespace btx