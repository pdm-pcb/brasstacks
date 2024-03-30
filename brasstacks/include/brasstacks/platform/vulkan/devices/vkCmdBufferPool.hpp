#ifndef VKLEARNIN_PLATFORM_VULKAN_DEVICES_VKCMDBUFFERPOOL_HPP
#define VKLEARNIN_PLATFORM_VULKAN_DEVICES_VKCMDBUFFERPOOL_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkCmdBufferPool final {
public:

    vkCmdBufferPool();
    ~vkCmdBufferPool();

    void create(uint32_t const queue_index,
                vk::CommandPoolCreateFlags const flags = { });

    void destroy();

    void reset(vk::CommandPoolResetFlags const flags = { }) const;

    inline auto const& native() const { return _handle; }

    vkCmdBufferPool(vkCmdBufferPool &&) = delete;
    vkCmdBufferPool(const vkCmdBufferPool &) = delete;

    vkCmdBufferPool & operator=(vkCmdBufferPool &&) = delete;
    vkCmdBufferPool & operator=(const vkCmdBufferPool &) = delete;

private:
    vk::CommandPool _handle;
    vk::Device      _device;
};

} // namespace btx

#endif // VKLEARNIN_PLATFORM_VULKAN_DEVICES_VKCMDBUFFERPOOL_HPP