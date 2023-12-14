#ifndef BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKLOGICALDEVICE_HPP
#define BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKLOGICALDEVICE_HPP

#include "brasstacks/pch.hpp"

#include "brasstacks/platform/vulkan/devices/vkCmdQueue.hpp"

namespace btx {

class vkPhysicalDevice;

class vkDevice final {
public:
    void submit(vk::SubmitInfo const &submit_info) const;
    void wait_idle() const;

    inline auto const& native()    const { return _handle; }
    inline auto const& cmd_queue() const { return _cmd_queue; }

    using Layers = std::vector<char const *>;
    vkDevice(vkPhysicalDevice const &adapter, Layers const &layers = { });
    ~vkDevice();

    vkDevice() = delete;

    vkDevice(vkDevice &&) = delete;
    vkDevice(const vkDevice &) = delete;

    vkDevice & operator=(vkDevice &&) = delete;
    vkDevice & operator=(const vkDevice &) = delete;

private:
    vkCmdQueue _cmd_queue;
    vk::Device _handle;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKLOGICALDEVICE_HPP