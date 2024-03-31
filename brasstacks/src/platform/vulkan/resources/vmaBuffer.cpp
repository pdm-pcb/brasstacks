#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/resources/vmaBuffer.hpp"

#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"

namespace btx {

// =============================================================================
vmaBuffer::vmaBuffer() :
    _handle        { nullptr },
    _memory_handle { nullptr },
    _size_bytes    { 0u },
    _alloc_info    { }
{ }

// =============================================================================
vmaBuffer::~vmaBuffer() {
    if(_handle != nullptr || _memory_handle != nullptr) {
        destroy();
    }
}

// =============================================================================
void vmaBuffer::create(vk::DeviceSize size_bytes,
                       vk::BufferUsageFlags const usage_flags,
                       ::VmaAllocationCreateFlags const create_flags,
                       vk::MemoryPropertyFlags const flags)
{
    if(_handle != nullptr) {
        BTX_CRITICAL("Buffer {} already exists", _handle);
    }

    _size_bytes = size_bytes;

    ::VkBufferCreateInfo const buffer_create_info {
        .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext                 = nullptr,
        .size                  = _size_bytes,
        .usage                 = ::VkBufferUsageFlags(usage_flags),
        .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0u,
        .pQueueFamilyIndices   = nullptr,
    };

    ::VmaAllocationCreateInfo const alloc_create_info {
        .flags = create_flags,
        .usage = VMA_MEMORY_USAGE_AUTO,
        .requiredFlags = ::VkMemoryPropertyFlags(flags),
        .preferredFlags = { },
        .memoryTypeBits = { },
        .pool = { },
        .pUserData = nullptr,
        .priority = 0.0f,
    };

    ::vmaCreateBuffer(
        vmaAllocator::native(),
        &buffer_create_info,
        &alloc_create_info,
        reinterpret_cast<::VkBuffer *>(&_handle),
        &_memory_handle,
        &_alloc_info
    );

    BTX_TRACE("Created buffer {} and device memory {:#x}",
              _handle, reinterpret_cast<uint64_t>(_memory_handle));
}

// =============================================================================
void vmaBuffer::destroy() {
    BTX_TRACE("Destroying buffer {}, freeing device memory {:#x}",
              _handle, reinterpret_cast<uint64_t>(_memory_handle));

    ::vmaDestroyBuffer(vmaAllocator::native(), _handle, _memory_handle);
    _handle = nullptr;
    _memory_handle = nullptr;
}

// =============================================================================
void vmaBuffer::fill_buffer(void const *data) const {
    void *mapped_memory_handle;
    auto const result = vk::Result(::vmaMapMemory(vmaAllocator::native(),
                                                  _memory_handle,
                                                  &mapped_memory_handle));
    if(result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to map buffer {} memory {:#x}: '{}'",
                     _handle, reinterpret_cast<uint64_t>(_memory_handle),
                     vk::to_string(result));
        return;
    }

    ::memcpy(mapped_memory_handle, data, _size_bytes);

    ::vmaUnmapMemory(vmaAllocator::native(), _memory_handle);
}

// =============================================================================
void vmaBuffer::send_to_device(void const *data) const {
    vmaBuffer staging_buffer;
    staging_buffer.create(
        _size_bytes,
        vk::BufferUsageFlagBits::eTransferSrc,
        (VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
         VMA_ALLOCATION_CREATE_MAPPED_BIT)
    );

    BTX_INFO("STAGING BUFFER {}", staging_buffer.native());

    ::memcpy(staging_buffer.alloc_info().pMappedData, data, _size_bytes);

    vk::BufferCopy const copy_region {
        .srcOffset = 0u,
        .dstOffset = 0u,
        .size = _size_bytes
    };

    vkCmdBuffer cmd_buffer;
    cmd_buffer.allocate(Renderer::device().transient_pool());
    cmd_buffer.begin_one_time_submit();

        cmd_buffer.native().copyBuffer(
            staging_buffer.native(),
            _handle,
            copy_region
        );

    cmd_buffer.end_recording();
    cmd_buffer.submit_and_wait_on_device();
    cmd_buffer.free();

    staging_buffer.destroy();
}

// =============================================================================
uint32_t vmaBuffer::_get_memory_type_index(vk::MemoryPropertyFlags const flags,
                                          vk::MemoryRequirements const &reqs)
{
    auto const &memory_props = vkPhysicalDevice::memory_properties();
    auto const type_count = memory_props.memoryTypeCount;

    // This bit-rithmetic bears some explanation. We're checking two bit fields
    // against our requirements for the memory itself.

    for(uint32_t type_index = 0u; type_index < type_count; ++type_index) {
        auto const type = memory_props.memoryTypes[type_index];

        // Each type index is actually a field in memoryTypeBits. If the index
        // we're currently on is enabled, that means we've found a matching
        // memory type.

        if((reqs.memoryTypeBits & (1u << type_index)) != 0u) {
            // The second check is against the memory properties. This can be
            // any combination of local to the CPU, local to the GPU, visible
            // to the CPU or not, and more.

            if(type.propertyFlags & flags) {
                return type_index;
            }
        }
    }

    BTX_CRITICAL("Could not find memory to match buffer requirements.");
    return std::numeric_limits<uint32_t>::max();
}

} // namespace btx