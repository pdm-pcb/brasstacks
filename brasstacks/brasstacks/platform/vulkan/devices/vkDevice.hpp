/**
 * @file vkDevice.hpp
 * @brief A class wrapping the Vulkan concept of a logical device
 */

#ifndef BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKDEVICE_HPP
#define BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKDEVICE_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkQueue;
class vkCmdBufferPool;

/**
 * @brief A class wrapping the Vulkan concept of a logical device
 *
 * This is a simple class given the simple use case for Vulkan's various
 * devices. While vkPhysicalDevice provides its features and extensions,
 * vkDevice is only worried about layers and its vkQueue.
 */
class vkDevice final {
public:
    vkDevice();
    ~vkDevice();

    /**
     * @brief Create the vkDevice object.
     */
    void create();
    void destroy();

    /**
     * @brief Convenience wrapper for waiting on device idle
     */
    void wait_idle() const;

    /**
     * @brief Return the native Vulkan device handle
     * @return vk::Device const&
     */
    inline auto const & native() const { return _handle; }

    /**
     * @brief Return the device queue
     * @return vkQueue const&
     */
    inline auto const & graphics_queue()  const { return *_graphics_queue; }

    inline auto const & transient_pool() const { return *_transient_pool; }

    vkDevice(vkDevice &&) = delete;
    vkDevice(const vkDevice &) = delete;

    vkDevice & operator=(vkDevice &&) = delete;
    vkDevice & operator=(const vkDevice &) = delete;

private:
    /**
     * @brief Native Vulkan device handle
     */
    vk::Device _handle;

    vkQueue *_graphics_queue;
    vkCmdBufferPool *_transient_pool;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKDEVICE_HPP