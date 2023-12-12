#ifndef VKLEARNIN_PLATFORM_VULKAN_DEVICES_VKCMDPOOL_HPP
#define VKLEARNIN_PLATFORM_VULKAN_DEVICES_VKCMDPOOL_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDevice;

class vkCmdPool final {
public:
    void reset(const vk::CommandPoolResetFlags flags = { }) const;

    void create(const vk::CommandPoolCreateFlags flags = { });
    void destroy();

    inline auto const& native() const { return _handle; }

    explicit vkCmdPool(vkDevice const &device);
    ~vkCmdPool() = default;

    vkCmdPool() = delete;

    vkCmdPool(vkCmdPool &&other) noexcept;
    vkCmdPool(const vkCmdPool &) = delete;

    vkCmdPool & operator=(vkCmdPool &&) = delete;
    vkCmdPool & operator=(const vkCmdPool &) = delete;

private:
    vk::CommandPool _handle;

    vkDevice const &_device;
};

} // namespace btx

#endif // VKLEARNIN_PLATFORM_VULKAN_DEVICES_VKCMDPOOL_HPP