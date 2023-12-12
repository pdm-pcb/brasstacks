#ifndef BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKLOGICALDEVICE_HPP
#define BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKLOGICALDEVICE_HPP

#include "brasstacks/pch.hpp"

#include "brasstacks/platform/vulkan/devices/vkCmdQueue.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdPool.hpp"

namespace btx {

class vkPhysicalDevice;

class vkLogicalDevice final {
public:
    void submit(vk::SubmitInfo const &submit_info) const;
    void wait_idle() const;

    inline auto const& native()    const { return _handle; }
    inline auto const& cmd_queue() const { return _cmd_queue; }

    inline auto& transient_pool()  { return _transient_pool; }

    using Layers = std::vector<char const *>;
    vkLogicalDevice(vkPhysicalDevice const &adapter,
                    Layers const &layers = { });
    ~vkLogicalDevice();

    vkLogicalDevice() = delete;

    vkLogicalDevice(vkLogicalDevice &&) = delete;
    vkLogicalDevice(const vkLogicalDevice &) = delete;

    vkLogicalDevice & operator=(vkLogicalDevice &&) = delete;
    vkLogicalDevice & operator=(const vkLogicalDevice &) = delete;

private:
    vkCmdQueue _cmd_queue;
    vkCmdPool  _transient_pool;
    vk::Device _handle;

    vkPhysicalDevice const &_adapter;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKLOGICALDEVICE_HPP