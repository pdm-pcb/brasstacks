/**
 * @file vkCmdPool.hpp
 * @brief A simple wrapper for a Vulkan command buffer
 */

#ifndef VKLEARNIN_PLATFORM_VULKAN_DEVICES_VKCMDPOOL_HPP
#define VKLEARNIN_PLATFORM_VULKAN_DEVICES_VKCMDPOOL_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDevice;

/**
 * @brief A simple wrapper for a Vulkan command buffer
 */
class vkCmdPool final {
public:

    /**
     * @brief Construct the vkCmdPool object.
     * @param device An established Vulkan logical device
     * @param flags Flags influencing the creation of this command pool
     */
    vkCmdPool(vkDevice const &device,
              vk::CommandPoolCreateFlags const flags = { });

    ~vkCmdPool();

    /**
     * @brief Reset the command pool, returning all allocated buffers to their
     * default state
     * @param flags Flags influencing how this command pool will be reset
     */
    void reset(vk::CommandPoolResetFlags const flags = { }) const;

    /**
     * @brief Return the native Vulkan handle
     * @return vk::CommandPool const&
     */
    inline auto const& native() const { return _handle; }

    vkCmdPool() = delete;

    vkCmdPool(vkCmdPool &&) = delete;
    vkCmdPool(const vkCmdPool &) = delete;

    vkCmdPool & operator=(vkCmdPool &&) = delete;
    vkCmdPool & operator=(const vkCmdPool &) = delete;

private:
    /**
     * @brief The Vulkan logical device used to create this command poool
     */
    vkDevice const &_device;

    /**
     * @brief The native Vulkan handle
     */
    vk::CommandPool _handle;
};

} // namespace btx

#endif // VKLEARNIN_PLATFORM_VULKAN_DEVICES_VKCMDPOOL_HPP