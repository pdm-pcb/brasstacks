#ifndef BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKLOGICALDEVICE_HPP
#define BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKLOGICALDEVICE_HPP

#include "brasstacks/pch.hpp"

#include "brasstacks/platform/vulkan/devices/VkCmdQueue.hpp"
#include "brasstacks/platform/vulkan/devices/VkCmdPool.hpp"

namespace btx {

class VkLogicalDevice final {
public:
    static void create(std::vector<char const *> const &layer_names = { });
    static void destroy();

    static void submit_to_cmd_queue(vk::SubmitInfo const &submit_info);
    static void wait_idle();

    inline static auto const& native()    { return _logical_device; }
    inline static auto const& cmd_queue() { return _cmd_queue;      }
    inline static auto& transient_pool()  { return _transient_pool; }

    VkLogicalDevice() = delete;

private:
    static VkCmdQueue _cmd_queue;
    static VkCmdPool  _transient_pool;
    static vk::Device _logical_device;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKLOGICALDEVICE_HPP