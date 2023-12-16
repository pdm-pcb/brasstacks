#ifndef BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKFRAME_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKFRAME_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDevice;
class vkCmdPool;
class vkCmdBuffer;
class vkFramebuffer;

class vkFrame final {
 public:
    void wait_and_reset() const;

    inline auto & image_acquire_semaphore() { return _image_acquire_sem; }

    inline auto const & cmds_complete_semaphore() const {
        return _cmds_complete_sem;
    }

    explicit vkFrame(vkDevice const &device);
    ~vkFrame();

    vkFrame() = delete;

    vkFrame(vkFrame &&) = delete;
    vkFrame(const vkFrame &) = delete;

    vkFrame & operator=(vkFrame &&) = delete;
    vkFrame & operator=(const vkFrame &) = delete;

private:
    vkDevice const &_device;

    vk::Fence     _queue_fence;
    vk::Semaphore _image_acquire_sem;
    vk::Semaphore _cmds_complete_sem;

    vkCmdPool   *_cmd_pool;
    vkCmdBuffer *_cmd_buffer;

    vkFramebuffer *_framebuffer;

    void _create_sync_primitives();
    void _create_cmd_buffer();
    void _create_framebuffer();
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKFRAME_HPP