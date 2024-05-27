#ifndef BRASSTACKS_PLATFORM_VULKAN_SWAPCHAIN_VKFRAMESYNC_HPP
#define BRASSTACKS_PLATFORM_VULKAN_SWAPCHAIN_VKFRAMESYNC_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkCmdBufferPool;
class vkCmdBuffer;

class vkFrameSync final {
 public:
    vkFrameSync();
    ~vkFrameSync();

    void create_sync_primitives();
    void create_cmd_structures();

    void destroy_sync_primitives();
    void destroy_cmd_structures();

    void wait_and_reset() const;

    inline auto const & queue_fence() const { return _queue_fence; }
    inline auto & present_semaphore() { return _present_sem; }
    inline auto const & present_semaphore() const { return _present_sem; }
    inline auto const & queue_semaphore() const {
        return _queue_sem;
    }
    inline auto const & cmd_buffer()  const { return *_cmd_buffer; }

    vkFrameSync(vkFrameSync &&rhs);
    vkFrameSync(const vkFrameSync &) = delete;

    vkFrameSync & operator=(vkFrameSync &&) = delete;
    vkFrameSync & operator=(const vkFrameSync &) = delete;

private:
    vk::Fence     _queue_fence;
    vk::Semaphore _present_sem;
    vk::Semaphore _queue_sem;

    vkCmdBufferPool *_cmd_buffer_pool;
    vkCmdBuffer     *_cmd_buffer;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_SWAPCHAIN_VKFRAMESYNC_HPP