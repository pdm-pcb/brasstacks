#ifndef BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKBUFFER_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKBUFFER_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkBuffer final {
public:
    vkBuffer();
    ~vkBuffer();

    void create(vk::DeviceSize size_bytes,
                vk::BufferUsageFlags const usage_flags);
    void destroy();

    void allocate(vk::MemoryPropertyFlags const flags);
    void free();

    void fill_buffer(void const *data) const;
    void send_to_device(void const *data) const;

    inline auto const & native() const { return _handle; }

    vkBuffer(vkBuffer &&) = delete;
    vkBuffer(vkBuffer const &) = delete;

    vkBuffer & operator=(vkBuffer &&) = delete;
    vkBuffer & operator=(vkBuffer const &) = delete;

private:
    vk::Buffer       _handle;
    vk::DeviceMemory _memory_handle;
    vk::DeviceSize   _size_bytes;
    vk::Device       _device;

    static uint32_t _get_memory_type_index(vk::MemoryPropertyFlags const flags,
                                           vk::MemoryRequirements const &reqs);
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKBUFFER_HPP