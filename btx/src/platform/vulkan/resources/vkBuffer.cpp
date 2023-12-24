#include "brasstacks/core.hpp"
#include "brasstacks/platform/vulkan/vulkan_formatters.hpp"
#include "brasstacks/platform/vulkan/resources/vkBuffer.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"

namespace btx {

vk::PhysicalDeviceMemoryProperties vkBuffer::_memory_props;

// =============================================================================
vkBuffer::vkBuffer(vkDevice const &device, vk::DeviceSize size_bytes,
                   vk::BufferUsageFlags const usage_flags,
                   vk::MemoryPropertyFlags const memory_flags) :
    _device      { device },
    _size_bytes  { size_bytes }
{
    if(_memory_props.memoryHeapCount == 0) {
        BTX_CRITICAL("vkBuffer::_memory_props must be set before creating a "
                     "device buffer.");
    }

    vk::BufferCreateInfo const create_info {
        .size        = _size_bytes,
        .usage       = usage_flags,
        .sharingMode = vk::SharingMode::eExclusive,

        // Why should a generic block of memory care about device queue
        // families, you might ask? Provided this structure assures Vulkan we
        // do not intend to share this buffer between threads, the below are
        // simply ignored.
        .queueFamilyIndexCount = 0u,
        .pQueueFamilyIndices   = nullptr,
    };

    auto const result = _device.native().createBuffer(create_info);
    if(result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to create buffer: '{}'",
                     vk::to_string(result.result));
        return;
    }

    _handle = result.value;
    BTX_TRACE("Created buffer {}", _handle);

    _allocate(memory_flags);
}

// =============================================================================
vkBuffer::~vkBuffer() {
    BTX_TRACE("Freeing memory {} and destroying buffer {}", _memory, _handle);

    _device.native().destroyBuffer(_handle);
    _device.native().freeMemory(_memory);
}

// =============================================================================
void
vkBuffer::set_memory_props(vk::PhysicalDeviceMemoryProperties const &props) {
    _memory_props = props;
}

// =============================================================================
void vkBuffer::fill_buffer(void const *data) const {
    void *mapped_memory;

    auto const result = _device.native().mapMemory(_memory, 0, VK_WHOLE_SIZE,
                                                   { }, &mapped_memory);
    if(result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to map buffer {} memory {}", _handle, _memory);
    }

    ::memcpy(mapped_memory, data, _size_bytes);

    _device.native().unmapMemory(_memory);
}

// =============================================================================
void vkBuffer::send_to_device(void const *data) const {
    vkBuffer const staging_buffer(
        _device, _size_bytes,
        vk::BufferUsageFlagBits::eTransferSrc,
        (vk::MemoryPropertyFlagBits::eHostVisible |
         vk::MemoryPropertyFlagBits::eHostCoherent)
    );

    staging_buffer.fill_buffer(data);

    vk::BufferCopy const copy_region {
        .srcOffset = 0u,
        .dstOffset = 0u,
        .size = _size_bytes
    };

    vkCmdBuffer const cmd_buffer(_device, _device.transient_pool());
    cmd_buffer.begin_one_time_submit();

        cmd_buffer.native().copyBuffer(
            staging_buffer.native(),
            _handle,
            copy_region
        );

    cmd_buffer.end_recording();
    cmd_buffer.submit_and_wait_on_device();
}

// =============================================================================
void vkBuffer::_allocate(vk::MemoryPropertyFlags const flags) {
    // The first order of business is to query the logical device about what
    // available memory matches properties we've specified thus far. A zero-
    // initialized vk::MemoryRequirements structure indicates that the
    // allocation we're after is the whole size of the buffer we've already
    // described with no offset.
    vk::MemoryRequirements mem_reqs { };
    _device.native().getBufferMemoryRequirements(_handle, &mem_reqs);

    // This function call will check the joint requirements of ourselves and
    // the logical device against the types of memory offered by the physical
    // device.
    auto const type_index = _get_memory_type_index(flags, mem_reqs);

    // Once a suitable memory type (and its index) is located, we're ready to
    // actually allocate the buffer.
    const vk::MemoryAllocateInfo alloc_info {
        .allocationSize  = mem_reqs.size,
        .memoryTypeIndex = type_index,
    };

    auto const alloc_result = _device.native().allocateMemory(alloc_info);
    if(alloc_result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to allocate {} bytes for buffer {}: '{}'",
                     _size_bytes, _handle, vk::to_string(alloc_result.result));
        return;
    }

    _memory = alloc_result.value;
    BTX_TRACE("\n\tAllocated {} bytes: Device memory {}"
              "\n\tFor buffer {}",
              _size_bytes, _memory, _handle);

    auto const bind_result = _device.native().bindBufferMemory(_handle,
                                                               _memory, 0u);
    if(bind_result != vk::Result::eSuccess) {
        BTX_TRACE("Failed to bind device memory {} to buffer {}: '{}'",
                  _memory, _handle, vk::to_string(bind_result));
    }
}

// =============================================================================
uint32_t vkBuffer::_get_memory_type_index(vk::MemoryPropertyFlags const flags,
                                          vk::MemoryRequirements const &reqs)
{
    auto const type_count = _memory_props.memoryTypeCount;

    // This bit-rithmetic bears some explanation. We're checking two bit fields
    // against our requirements for the memory itself.

    for(uint32_t type_index = 0u; type_index < type_count; ++type_index) {
        auto const type = _memory_props.memoryTypes[type_index];

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