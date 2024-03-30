#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/vkAllocator.hpp"

#include "brasstacks/platform/vulkan/vkInstance.hpp"
#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

namespace btx {

::VmaAllocator vkAllocator::_handle { nullptr };

// =============================================================================
void vkAllocator::create(uint32_t const api_version) {
    if(_handle != nullptr ) {
        BTX_CRITICAL("Allocator {:#x} already exists",
                     reinterpret_cast<uint64_t>(_handle));
    }

    ::VmaVulkanFunctions vulkan_functions { };
    vulkan_functions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
    vulkan_functions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

    ::VmaAllocatorCreateInfo const create_info {
        .flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT,
        .physicalDevice = vkPhysicalDevice::native(),
        .device = Renderer::device().native(),
        .preferredLargeHeapBlockSize = 0u,
        .pAllocationCallbacks = nullptr,
        .pDeviceMemoryCallbacks = nullptr,
        .pHeapSizeLimit = nullptr,
        .pVulkanFunctions = &vulkan_functions,
        .instance = vkInstance::native(),
        .vulkanApiVersion = api_version,
        .pTypeExternalMemoryHandleTypes = nullptr,
    };

    auto const result =
        vk::Result(::vmaCreateAllocator(&create_info, &_handle));

    if(result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to create VMA allocator: '{}'",
                     vk::to_string(result));
        return;
    }

    BTX_TRACE("Created allocator {:#x}", reinterpret_cast<uint64_t>(_handle));
}

// =============================================================================
void vkAllocator::destroy() {
    BTX_TRACE("Destroying allocator {:#x}",
              reinterpret_cast<uint64_t>(_handle));

    ::vmaDestroyAllocator(_handle);
    _handle = nullptr;
}

} // namespace btx