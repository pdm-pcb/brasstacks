#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/VkDebugger.hpp"

#include "brasstacks/platform/vulkan/VkInstance.hpp"

namespace btx {

vk::DebugUtilsMessengerEXT VkDebugger::_debug_messenger { };

// =============================================================================
VKAPI_ATTR vk::Bool32 VKAPI_CALL VkDebugger::messenger(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT types,
        const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
        [[maybe_unused]] void *user_data)
{
    switch(severity) {
        case ::VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            BTX_TRACE("{:s}", callback_data->pMessage);
            break;
        case ::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            BTX_INFO("{:s}", callback_data->pMessage);
            break;
        case ::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            if(callback_data->messageIdNumber == 0xb3d4346b ||
               callback_data->messageIdNumber == 0xdc18ad6b)
            {
                break;
            }
            BTX_WARN("{:s}", callback_data->pMessage);
            break;
        case ::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            BTX_ERROR("\n{:s}\n", callback_data->pMessage);
            assert(false);
            break;
        case ::VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
            BTX_CRITICAL("???: {:s}", callback_data->pMessage);
            break;
    }

    return VK_FALSE;
}

// =============================================================================
void VkDebugger::init(vk::Instance &instance) {
    // Populate the create info struct with the severity levels we're
    // interested in, the types we're interested in, and offer a callback
    // pointer to the API
    const vk::DebugUtilsMessengerCreateInfoEXT messenger_info {
        .messageSeverity = (
#if defined(BTX_LINUX)
			// I find this log level too verbose (ha) on Windows, but it can
            // be helpful on Linux
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
#endif
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
        .pfnUserCallback = VkDebugger::messenger
    };

    // Give it a shot
    auto const result = instance.createDebugUtilsMessengerEXT(
        &messenger_info,
        nullptr,
        &_debug_messenger
    );

    // React accordingly
    if(result != vk::Result::eSuccess) {
        BTX_CRITICAL(
            "Unable to create debug messenger: '{}'",
            to_string(result)
        );
    }
    else {
        BTX_TRACE(
            "Created VkDebugger messenger {:#x}",
            reinterpret_cast<std::uint64_t>(
                VkDebugUtilsMessengerEXT(_debug_messenger)
            )
        );
    }
}

// =============================================================================
void VkDebugger::shutdown(vk::Instance &instance) {
    BTX_TRACE(
        "Destroying VkDebugger messenger {:#x}",
            reinterpret_cast<std::uint64_t>(
                VkDebugUtilsMessengerEXT(_debug_messenger)
            )
    );
    instance.destroy(_debug_messenger);
}

} // namespace btx