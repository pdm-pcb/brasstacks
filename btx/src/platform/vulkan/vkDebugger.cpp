#include "brasstacks/core.hpp"
#include "brasstacks/platform/vulkan/vkDebugger.hpp"

#include "brasstacks/platform/vulkan/vkInstance.hpp"

namespace btx {

vk::DebugUtilsMessengerEXT vkDebugger::_debug_messenger { };

// =============================================================================
VKAPI_ATTR vk::Bool32 VKAPI_CALL vkDebugger::messenger(
        ::VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        [[maybe_unused]] ::VkDebugUtilsMessageTypeFlagsEXT types,
        const ::VkDebugUtilsMessengerCallbackDataEXT *callback_data,
        [[maybe_unused]] void *user_data)
{
    if((severity & ::VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) != 0) {
        BTX_TRACE("{}", callback_data->pMessage);
    }
    if((severity & ::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) != 0) {
        BTX_INFO("{}", callback_data->pMessage);
    }
    if((severity & ::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) != 0) {
        BTX_WARN("{}", callback_data->pMessage);
    }
    if((severity & ::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) != 0) {
        BTX_ERROR("\n{}\n", callback_data->pMessage);
    }

    return VK_FALSE;
}

// =============================================================================
void vkDebugger::init(vk::Instance &instance) {
    // Populate the create info struct with the severity levels we're
    // interested in, the types we're interested in, and offer a callback
    // pointer to the API
    const vk::DebugUtilsMessengerCreateInfoEXT messenger_info {
        .messageSeverity = (
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo    |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
        ),
        .messageType = (
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral     |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation  |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding
        ),
        .pfnUserCallback = vkDebugger::messenger
    };

    // Give it a shot
    auto const result = instance.createDebugUtilsMessengerEXT(
        &messenger_info,
        nullptr,
        &_debug_messenger
    );

    // React accordingly
    if(result != vk::Result::eSuccess) {
        BTX_CRITICAL("Unable to create debug messenger: '{}'",
                     vk::to_string(result));
    }
    else {
        BTX_TRACE(
            "Created vkDebugger messenger {:#x}",
            reinterpret_cast<uint64_t>(
                ::VkDebugUtilsMessengerEXT(_debug_messenger)
            )
        );
    }
}

// =============================================================================
void vkDebugger::shutdown(vk::Instance &instance) {
    BTX_TRACE(
        "Destroying vkDebugger messenger {:#x}",
            reinterpret_cast<uint64_t>(
                ::VkDebugUtilsMessengerEXT(_debug_messenger)
            )
    );
    instance.destroy(_debug_messenger);
}

} // namespace btx