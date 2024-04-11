/**
 * @file vkDebugger.hpp
 * @brief A class providing a callback function for the Vulkan validation layer.
 */

#ifndef BRASSTACKS_PLATFORM_VULKAN_VKDEBUGGER_HPP
#define BRASSTACKS_PLATFORM_VULKAN_VKDEBUGGER_HPP

#include "brasstacks/pch.hpp"

namespace btx {

/**
 * @brief A class providing a callback function for the Vulkan validation layer.
 */
class vkDebugger final {
public:
    static VKAPI_ATTR vk::Bool32 VKAPI_CALL messenger(
        ::VkDebugUtilsMessageSeverityFlagBitsEXT      severity,
        ::VkDebugUtilsMessageTypeFlagsEXT             types,
        const ::VkDebugUtilsMessengerCallbackDataEXT *callback_data,
        void                                         *user_data
    );

    static void create();
    static void destroy();

    vkDebugger() = delete;

private:
    static vk::DebugUtilsMessengerEXT _handle;
    static vk::Instance _instance;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_VKDEBUGGER_HPP