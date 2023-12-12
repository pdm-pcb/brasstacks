#ifndef BRASSTACKS_PLATFORM_VULKAN_VKDEBUGGER_HPP
#define BRASSTACKS_PLATFORM_VULKAN_VKDEBUGGER_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDebugger final {
public:
    static VKAPI_ATTR vk::Bool32 VKAPI_CALL messenger(
        VkDebugUtilsMessageSeverityFlagBitsEXT      severity,
        VkDebugUtilsMessageTypeFlagsEXT             types,
        const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
        void                                       *user_data
    );

    static void init(vk::Instance &instance);
    static void shutdown(vk::Instance &instance);

    vkDebugger() = delete;

private:
    static vk::DebugUtilsMessengerEXT _debug_messenger;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_VKDEBUGGER_HPP