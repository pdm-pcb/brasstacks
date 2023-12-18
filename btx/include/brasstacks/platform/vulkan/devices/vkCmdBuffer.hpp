/**
 * @file vkCmdBuffer.hpp
 * @brief A simple wrapper for a Vulkan command buffer
 */

#ifndef BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKCMDBUFFER_HPP
#define BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKCMDBUFFER_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDevice;
class vkCmdPool;

/**
 * @brief A simple wrapper for a Vulkan command buffer
 */
class vkCmdBuffer final {
public:

    /**
     * @brief Construct the vkCmdBuffer object.
     * @param device An established Vulkan logical device
     * @param pool The command pool from which this buffer will be allocated
     */
    vkCmdBuffer(vkDevice const &device, vkCmdPool const &pool);

    ~vkCmdBuffer();

    /**
     * @brief Begin recording commands
     */
    void begin_one_time_submit() const;

    /**
     * @brief Stop recording commands
     */
    void end_recording() const;

    /**
     * @brief Begin a specified render pass
     * @param info The details about the render pass we're starting
     */
    void begin_render_pass(vk::RenderPassBeginInfo const &info) const;

    /**
     * @brief Stop recording commands for the in-progress render pass
     */
    void end_render_pass() const;

    void submit_and_wait_on_device() const;

    /**
     * @brief Return the native Vulkan handle
     * @return vk::CommandBuffer const&
     */
    inline auto const& native() const { return _handle; }

    vkCmdBuffer() = delete;

    vkCmdBuffer(vkCmdBuffer &&) = delete;
    vkCmdBuffer(const vkCmdBuffer &) = delete;

    vkCmdBuffer & operator=(vkCmdBuffer &&) = delete;
    vkCmdBuffer & operator=(const vkCmdBuffer &) = delete;

private:
    /**
     * @brief Device used to create this command buffer
     */
    vkDevice  const &_device;

    /**
     * @brief Pool from which this command buffer was allocated
     */
    vkCmdPool const &_pool;

    /**
     * @brief Native Vulkan handle
     */
    vk::CommandBuffer _handle;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKCMDBUFFER_HPP