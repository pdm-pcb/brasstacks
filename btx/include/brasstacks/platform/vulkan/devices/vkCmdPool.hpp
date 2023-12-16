#ifndef VKLEARNIN_PLATFORM_VULKAN_DEVICES_VKCMDPOOL_HPP
#define VKLEARNIN_PLATFORM_VULKAN_DEVICES_VKCMDPOOL_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDevice;

class vkCmdPool final {
public:
    void reset(vk::CommandPoolResetFlags const flags = { }) const;

    inline auto const& native() const { return _handle; }

    vkCmdPool(vkDevice const &device,
              vk::CommandPoolCreateFlags const flags = { });
    ~vkCmdPool();

    vkCmdPool() = delete;

    vkCmdPool(vkCmdPool &&) = delete;
    vkCmdPool(const vkCmdPool &) = delete;

    vkCmdPool & operator=(vkCmdPool &&) = delete;
    vkCmdPool & operator=(const vkCmdPool &) = delete;

private:
    vkDevice const &_device;

    vk::CommandPool _handle;
};

} // namespace btx

#endif // VKLEARNIN_PLATFORM_VULKAN_DEVICES_VKCMDPOOL_HPP