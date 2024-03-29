#ifndef BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKBUFFER_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKBUFFER_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDevice;

class vkBuffer final {
public:
    vkBuffer(vkDevice const &device, vk::DeviceSize size_bytes,
             vk::BufferUsageFlags const usage_flags,
             vk::MemoryPropertyFlags const memory_flags);
    ~vkBuffer();

    void fill_buffer(void const *data) const;
    void send_to_device(void const *data) const;

    inline auto const & native() const { return _handle; }

    vkBuffer() = delete;

    vkBuffer(vkBuffer &&) = delete;
    vkBuffer(vkBuffer const &) = delete;

    vkBuffer & operator=(vkBuffer &&) = delete;
    vkBuffer & operator=(vkBuffer const &) = delete;

private:
    vkDevice const &_device;

    vk::Buffer       _handle;
    vk::DeviceSize   _size_bytes;
    vk::DeviceMemory _memory;

    void _allocate(vk::MemoryPropertyFlags const flags);

    static uint32_t _get_memory_type_index(vk::MemoryPropertyFlags const flags,
                                           vk::MemoryRequirements const &reqs);
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKBUFFER_HPP