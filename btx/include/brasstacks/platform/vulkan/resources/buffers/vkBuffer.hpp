#ifndef BRASSTACKS_PLATFORM_VULKAN_RESOURCES_BUFFERS_VKBUFFER_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RESOURCES_BUFFERS_VKBUFFER_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDevice;
class vkPhysicalDevice;

class vkBuffer {
public:
    vkBuffer(vkDevice const &device, size_t size_bytes,
             vk::BufferUsageFlags const usage_flags,
             vk::PhysicalDeviceMemoryProperties const &memory_props,
             vk::MemoryPropertyFlags const memory_flags);
    ~vkBuffer();

    void fill_buffer(void *data, size_t const size_bytes);

    inline auto const & native() const { return _handle; }

    vkBuffer() = delete;

    vkBuffer(vkBuffer &&) = delete;
    vkBuffer(vkBuffer const &) = delete;

    vkBuffer & operator=(vkBuffer &&) = delete;
    vkBuffer & operator=(vkBuffer const &) = delete;

private:
    vkDevice const &_device;

    vk::Buffer       _handle;
    size_t           _size_bytes;
    vk::DeviceMemory _memory;

    void _allocate(vk::PhysicalDeviceMemoryProperties const &props,
                   vk::MemoryPropertyFlags const flags);

    uint32_t _get_memory_type_index(
        vk::PhysicalDeviceMemoryProperties const &props,
        vk::MemoryPropertyFlags const flags,
        vk::MemoryRequirements const &reqs);
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RESOURCES_BUFFERS_VKBUFFER_HPP