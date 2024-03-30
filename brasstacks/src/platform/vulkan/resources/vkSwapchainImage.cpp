#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/resources/vkSwapchainImage.hpp"

#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"
#include "brasstacks/platform/vulkan/resources/vkBuffer.hpp"

namespace btx {

// =============================================================================
vkSwapchainImage::vkSwapchainImage() :
    _handle        { nullptr },
    _format        { vk::Format::eUndefined }
{ }

// =============================================================================
void vkSwapchainImage::create(vk::Image const &handle, vk::Format const format) {
    if(_handle != nullptr) {
        BTX_CRITICAL("Swapchain image {} already exists", _handle);
    }

    _handle = handle;
    _format = format;
}

} // namespace btx