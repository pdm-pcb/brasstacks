#ifndef BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VMABUFFER_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VMABUFFER_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vmaBuffer final {
public:
    vmaBuffer();
    ~vmaBuffer();

    void create(vk::DeviceSize size_bytes,
                vk::BufferUsageFlags const usage_flags,
                ::VmaAllocationCreateFlags const create_flags,
                vk::MemoryPropertyFlags const flags = { });
    void destroy();

    void fill_buffer(void const *data) const;
    void send_to_device(void const *data) const;

    inline auto const & native() const { return _handle; }
    inline auto const & alloc_info() const { return _alloc_info; }

    vmaBuffer(vmaBuffer &&) = delete;
    vmaBuffer(vmaBuffer const &) = delete;

    vmaBuffer & operator=(vmaBuffer &&) = delete;
    vmaBuffer & operator=(vmaBuffer const &) = delete;

private:
    vk::Buffer          _handle;
    ::VmaAllocation     _memory_handle;
    vk::DeviceSize      _size_bytes;
    ::VmaAllocationInfo _alloc_info;

    static uint32_t _get_memory_type_index(vk::MemoryPropertyFlags const flags,
                                           vk::MemoryRequirements const &reqs);
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VMABUFFER_HPP