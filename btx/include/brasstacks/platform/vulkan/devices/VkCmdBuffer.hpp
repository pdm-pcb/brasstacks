#ifndef BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKCMDBUFFER_HPP
#define BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKCMDBUFFER_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class VkCmdBuffer final {
public:
    void begin_one_time_submit() const;
    void end_recording() const;

    void begin_render_pass(vk::RenderPassBeginInfo const &info) const;
    void end_render_pass() const;

    void submit_and_wait_on_device() const;

    void allocate(const vk::CommandPool pool, const bool primary = true);
    void free();

    inline auto const& native() const { return _buffer; }

    VkCmdBuffer();
    ~VkCmdBuffer() = default;

    VkCmdBuffer(VkCmdBuffer &&other) noexcept;
    VkCmdBuffer(const VkCmdBuffer &) = delete;

    VkCmdBuffer & operator=(VkCmdBuffer &&) = delete;
    VkCmdBuffer & operator=(const VkCmdBuffer &) = delete;

private:
    vk::CommandPool   _pool;
    vk::CommandBuffer _buffer;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKCMDBUFFER_HPP