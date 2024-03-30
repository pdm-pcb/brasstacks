#ifndef BRASSTACKS_PLATFORM_VULKAN_VMAALLOCATOR_HPP
#define BRASSTACKS_PLATFORM_VULKAN_VMAALLOCATOR_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkAllocator {
public:
    static void create(uint32_t const api_version);
    static void destroy();

    vkAllocator() = delete;
    ~vkAllocator() = delete;

    vkAllocator(vkAllocator &&) = delete;
    vkAllocator(vkAllocator const &) = delete;

    vkAllocator & operator=(vkAllocator &&) = delete;
    vkAllocator & operator=(vkAllocator const &) = delete;

private:
    static ::VmaAllocator _handle;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_VMAALLOCATOR_HPP