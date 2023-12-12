#ifndef BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKCMDBUFFER_HPP
#define BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKCMDBUFFER_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkLogicalDevice;

class vkCmdBuffer final {
public:
    void begin_one_time_submit() const;
    void end_recording() const;

    void begin_render_pass(vk::RenderPassBeginInfo const &info) const;
    void end_render_pass() const;

    void submit_and_wait_on_device() const;

    void allocate(const vk::CommandPool pool, const bool primary = true);
    void free();

    inline auto const& native() const { return _handle; }

    explicit vkCmdBuffer(vkLogicalDevice const &device);
    ~vkCmdBuffer() = default;

    vkCmdBuffer() = delete;

    vkCmdBuffer(vkCmdBuffer &&other) noexcept;
    vkCmdBuffer(const vkCmdBuffer &) = delete;

    vkCmdBuffer & operator=(vkCmdBuffer &&) = delete;
    vkCmdBuffer & operator=(const vkCmdBuffer &) = delete;

private:
    vk::CommandPool   _pool;
    vk::CommandBuffer _handle;

    vkLogicalDevice const &_device;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKCMDBUFFER_HPP