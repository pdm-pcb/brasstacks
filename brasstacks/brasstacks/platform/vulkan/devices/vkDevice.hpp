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

class vkDevice final {
public:
    vkDevice();
    ~vkDevice();

    void create();
    void destroy();

    void wait_idle() const;

    inline auto const & native() const { return _handle; }
    inline auto const & graphics_queue() const { return *_graphics_queue; }
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