#ifndef BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKCMDBUFFER_HPP
#define BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKCMDBUFFER_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkCmdBufferPool;

class vkCmdBuffer final {
public:
    vkCmdBuffer();
    ~vkCmdBuffer() = default;

    void allocate(vkCmdBufferPool const &pool);
    void free();

    void begin_one_time_submit() const;
    void end_recording() const;

    void begin_rendering(vk::RenderingInfoKHR const &info) const;
    void end_rendering() const;

    void submit_and_wait_on_device() const;

    inline auto const& native() const { return _handle; }

    vkCmdBuffer(vkCmdBuffer &&rhs);
    vkCmdBuffer(const vkCmdBuffer &) = delete;

    vkCmdBuffer & operator=(vkCmdBuffer &&) = delete;
    vkCmdBuffer & operator=(const vkCmdBuffer &) = delete;

private:
    vk::CommandBuffer _handle;
    vk::CommandPool   _pool;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKCMDBUFFER_HPP