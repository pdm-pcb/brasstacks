#ifndef BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKCMDBUFFER_HPP
#define BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKCMDBUFFER_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDevice;
class vkCmdPool;

class vkCmdBuffer final {
public:

    vkCmdBuffer(vkCmdPool const &pool);
    ~vkCmdBuffer();

    void begin_one_time_submit() const;
    void end_recording() const;

    void begin_render_pass(vk::RenderPassBeginInfo const &info) const;
    void end_render_pass() const;

    void submit_and_wait_on_device() const;

    inline auto const& native() const { return _handle; }

    vkCmdBuffer() = delete;

    vkCmdBuffer(vkCmdBuffer &&) = delete;
    vkCmdBuffer(const vkCmdBuffer &) = delete;

    vkCmdBuffer & operator=(vkCmdBuffer &&) = delete;
    vkCmdBuffer & operator=(const vkCmdBuffer &) = delete;

private:
    vkCmdPool const &_pool;

    vk::CommandBuffer _handle;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKCMDBUFFER_HPP