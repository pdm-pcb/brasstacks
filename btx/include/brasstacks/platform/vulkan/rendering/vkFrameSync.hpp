/**
 * @file vkFrameSync.hpp
 * @brief A container for various frame-related data
 */

#ifndef BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKFRAME_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKFRAME_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDevice;
class vkCmdPool;
class vkCmdBuffer;

/**
 * @brief A container for various frame-related data
 *
 * This class is used to help synchronize the device queue swapchain images
 * that are flying around at any given moment. To that end, it has a dedicated
 * command pool and corresponding buffer set up for one-time submissions.
 */
class vkFrameSync final {
 public:

    /**
     * @brief Construct the vkFrameSync object.
     * @param device An established Vulkan logical device
     */
    vkFrameSync(vkDevice const &device);

    ~vkFrameSync();

    /**
     * @brief Wait on and then reset this frame's device queue fence
     */
    void wait_and_reset() const;

    /**
     * @brief Return this frame's device queue fence
     * @return vk::Fence const&
     */
    inline auto const & queue_fence() const { return _queue_fence; }

    /**
     * @brief Return the last semaphore used to acquire a swapchain image
     * @return vk::Semaphore&
     */
    inline auto & image_acquire_semaphore() { return _image_acquire_sem; }

    /**
     * @brief Return this frame's command batch complete semaphore
     * @return vk::Semaphore const&
     */
    inline auto const & cmds_complete_semaphore() const {
        return _cmds_complete_sem;
    }

    /**
     * @brief Return this frame's command buffer
     * @return vkCmdBuffer const&
     */
    inline auto const & cmd_buffer()  const { return *_cmd_buffer; }

    vkFrameSync() = delete;

    vkFrameSync(vkFrameSync &&) = delete;
    vkFrameSync(const vkFrameSync &) = delete;

    vkFrameSync & operator=(vkFrameSync &&) = delete;
    vkFrameSync & operator=(const vkFrameSync &) = delete;

private:
    /**
     * @brief The Vulkan logical device tying all of the member variables
     * together
     */
    vkDevice const &_device;

    /**
     * @brief A fence that's signaled when this frame's command buffer has
     * completed its work
     */
    vk::Fence _queue_fence;

    /**
     * @brief A semaphore that's signaled when the swapchain fulfills an image
     * acquisition request.
     *
     * This semaphore will be one of a pool of semaphores managed by the
     * Renderer backend. They'll be recycled between all vkFrameSyncs that request
     * images from the swapchain.
     */
    vk::Semaphore _image_acquire_sem;

    /**
     * @brief A semaphore that's signaled when this frame's command buffer has
     * completed its work
     */
    vk::Semaphore _cmds_complete_sem;

    /**
     * @brief This frame's command pool, created with the transient bit
     */
    vkCmdPool *_cmd_pool;

    /**
     * @brief This frame's command buffer, used exclusively for one-time
     * submissions
     */
    vkCmdBuffer *_cmd_buffer;

    /**
     * @brief Creates the fence and semaphores used by this frame
     */
    void _create_sync_primitives();

    /**
     * @brief Creates the command pool and allocates the command buffer used by
     * this frame
     */
    void _create_cmd_structures();
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKFRAME_HPP