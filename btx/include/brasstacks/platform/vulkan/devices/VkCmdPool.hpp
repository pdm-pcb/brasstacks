#ifndef VKLEARNIN_PLATFORM_VULKAN_DEVICES_VKCMDPOOL_HPP
#define VKLEARNIN_PLATFORM_VULKAN_DEVICES_VKCMDPOOL_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class VkCmdPool final {
public:
    void reset(const vk::CommandPoolResetFlags flags = { }) const;

    void create(const vk::CommandPoolCreateFlags flags = { });
    void destroy();

    inline auto const& native() const { return _pool; }

    VkCmdPool();
    ~VkCmdPool() = default;

    VkCmdPool(VkCmdPool &&other) noexcept;
    VkCmdPool(const VkCmdPool &) = delete;

    VkCmdPool & operator=(VkCmdPool &&) = delete;
    VkCmdPool & operator=(const VkCmdPool &) = delete;

private:
    vk::CommandPool _pool;
};

} // namespace btx

#endif // VKLEARNIN_PLATFORM_VULKAN_DEVICES_VKCMDPOOL_HPP