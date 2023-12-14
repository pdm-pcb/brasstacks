#ifndef BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKFRAME_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKFRAME_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdPool.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"

namespace btx {

class vkDevice;

class vkFrame final {
public:
    void wait_on_queue_fence() const;
    void submit_to_device() const;

    inline auto const & cmd_pool()   const { return _cmd_pool; }
    inline auto const & cmd_buffer() const { return _cmd_buffer; }

    inline auto const & acquire_complete_sem() const {
        return _acquire_complete;
    }

    inline auto const & commands_complete_sem() const {
        return _commands_complete;
    }

    inline auto const & queue_complete_fence() const {
        return _queue_complete;
    }

    inline void set_image_index(uint32_t const index) { _image_index = index; }
    inline auto image_index() const { return _image_index; }

    explicit vkFrame(vkDevice const &device);
    ~vkFrame();

    vkFrame() = delete;

    vkFrame(vkFrame &&other);
    vkFrame(vkFrame const &) = delete;

    vkFrame & operator=(vkFrame &&) = delete;
    vkFrame & operator=(vkFrame const &) = delete;

private:
    vkDevice const &_device;

    vkCmdPool   _cmd_pool;
    vkCmdBuffer _cmd_buffer;

    vk::Semaphore _acquire_complete;
    vk::Semaphore _commands_complete;
    vk::Fence     _queue_complete;

    uint32_t _image_index;

    void _create_sync_primitives();
    void _destroy_sync_primitives();
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RENDERING_VKFRAME_HPP