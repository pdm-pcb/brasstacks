#ifndef BRASSTACKS_TOOLS_VKDEBUGGER_HPP
#define BRASSTACKS_TOOLS_VKDEBUGGER_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class VKDebugger final {
public:
    static VKAPI_ATTR vk::Bool32 VKAPI_CALL messenger(
        VkDebugUtilsMessageSeverityFlagBitsEXT      severity,
        VkDebugUtilsMessageTypeFlagsEXT             types,
        const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
        void                                       *user_data
    );

    static void init(vk::Instance &instance);
    static void shutdown(vk::Instance &instance);

    VKDebugger() = delete;

private:
    static vk::DebugUtilsMessengerEXT _debug_messenger;
};

} // namespace btx

#endif // BRASSTACKS_TOOLS_VKDEBUGGER_HPP