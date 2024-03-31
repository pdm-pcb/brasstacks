#ifndef BRASSTACKS_PLATFORM_VULKAN_VMAALLOCATOR_HPP
#define BRASSTACKS_PLATFORM_VULKAN_VMAALLOCATOR_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vmaAllocator {
public:
    static void create(uint32_t const api_version);
    static void destroy();

    static auto const & native() { return _handle; }

    vmaAllocator() = delete;
    ~vmaAllocator() = delete;

    vmaAllocator(vmaAllocator &&) = delete;
    vmaAllocator(vmaAllocator const &) = delete;

    vmaAllocator & operator=(vmaAllocator &&) = delete;
    vmaAllocator & operator=(vmaAllocator const &) = delete;

private:
    static ::VmaAllocator _handle;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_VMAALLOCATOR_HPP