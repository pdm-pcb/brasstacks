#ifndef BRASSTACKS_PLATFORM_VULKAN_RESOURCES_BUFFERS_VKBUFFER_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RESOURCES_BUFFERS_VKBUFFER_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDevice;

class vkBuffer {
public:
    vkBuffer(vkDevice const &device, vk::DeviceSize size_bytes,
             vk::BufferUsageFlags const usage_flags,
             vk::MemoryPropertyFlags const memory_flags);
    ~vkBuffer();

    static void set_memory_props(
        vk::PhysicalDeviceMemoryProperties const &props
    );

    void fill_buffer(void const *data);
    void send_to_device(void const *data);

    inline auto const & native() const { return _handle; }

    vkBuffer() = delete;

    vkBuffer(vkBuffer &&) = delete;
    vkBuffer(vkBuffer const &) = delete;

    vkBuffer & operator=(vkBuffer &&) = delete;
    vkBuffer & operator=(vkBuffer const &) = delete;

private:
    vkDevice const &_device;

    static vk::PhysicalDeviceMemoryProperties _memory_props;

    vk::Buffer       _handle;
    vk::DeviceSize   _size_bytes;
    vk::DeviceMemory _memory;

    void _allocate(vk::MemoryPropertyFlags const flags);

    uint32_t _get_memory_type_index(vk::MemoryPropertyFlags const flags,
                                    vk::MemoryRequirements const &reqs);
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RESOURCES_BUFFERS_VKBUFFER_HPP