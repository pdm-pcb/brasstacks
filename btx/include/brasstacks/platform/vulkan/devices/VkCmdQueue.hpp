#ifndef BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKCMDQUEUE_HPP
#define BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKCMDQUEUE_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDevice;

class vkCmdQueue final {
public:
    void fill_create_info(uint32_t const index, float const priority = 1.0f);
    void request_queue();

    inline auto const& native()      const { return _handle;      }
    inline auto const& index()       const { return _index;       }
    inline auto const& priority()    const { return _priority;    }
    inline auto const& create_info() const { return _create_info; }

    explicit vkCmdQueue(vkDevice const &device);
    ~vkCmdQueue() = default;

    vkCmdQueue() = delete;

    vkCmdQueue(vkCmdQueue &&) = delete;
    vkCmdQueue(const vkCmdQueue &) = delete;

    vkCmdQueue& operator=(vkCmdQueue &&) = delete;
    vkCmdQueue& operator=(const vkCmdQueue &) = delete;

private:
    uint32_t _index;
    float    _priority;

    vk::DeviceQueueCreateInfo _create_info;
    vk::Queue _handle;

    vkDevice const &_device;

};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_DEVICES_VKCMDQUEUE_HPP