#ifndef BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKCMDQUEUE_HPP
#define BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKCMDQUEUE_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class VkCmdQueue final {
public:
    void fill_create_info(uint32_t const index, float const priority = 1.0f);
    void request_queue();

    inline auto const& native()      const { return _queue;       }
    inline auto const& index()       const { return _index;       }
    inline auto const& priority()    const { return _priority;    }
    inline auto const& create_info() const { return _create_info; }

    VkCmdQueue();
    ~VkCmdQueue() = default;

    VkCmdQueue(VkCmdQueue &&) = delete;
    VkCmdQueue(const VkCmdQueue &) = delete;

    VkCmdQueue& operator=(VkCmdQueue &&) = delete;
    VkCmdQueue& operator=(const VkCmdQueue &) = delete;

private:
    uint32_t _index;
    float    _priority;
    vk::DeviceQueueCreateInfo _create_info;

    vk::Queue _queue;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKCMDQUEUE_HPP