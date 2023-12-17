#ifndef BRASSTACKS_PLATFORM_VULKAN_RESOURCES_BUFFERS_VKBUFFER_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RESOURCES_BUFFERS_VKBUFFER_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDevice;

class vkBuffer {
public:
    explicit vkBuffer(vkDevice const &device);
    ~vkBuffer();

    inline auto const & native() const { return _handle; }

    vkBuffer() = delete;

    vkBuffer(vkBuffer &&) = delete;
    vkBuffer(vkBuffer const &) = delete;

    vkBuffer & operator=(vkBuffer &&) = delete;
    vkBuffer & operator=(vkBuffer const &) = delete;

private:
    vkDevice const &_device;

    vk::Buffer       _handle;
    size_t           _size;
    vk::DeviceMemory _memory;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RESOURCES_BUFFERS_VKBUFFER_HPP