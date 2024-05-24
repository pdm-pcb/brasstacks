#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/resources/vkBuffer.hpp"

#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"

namespace btx {

// =============================================================================
vkBuffer::vkBuffer() :
    _handle        { nullptr },
    _memory_handle { nullptr },
    _size_bytes    { 0u },
    _device        { nullptr }
{ }

// =============================================================================
vkBuffer::~vkBuffer() {
    if(_handle) {
        destroy();
    }

    if(_memory_handle) {
        free();
    }
}

// =============================================================================
void vkBuffer::create(vk::DeviceSize size_bytes,
                      vk::BufferUsageFlags const usage_flags)
{
    if(_handle) {
        BTX_CRITICAL("Buffer {} already exists", _handle);
    }

    _device = Renderer::device().native();
    _size_bytes = size_bytes;

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

    _handle = _device.createBuffer(create_info);
    BTX_TRACE("Created buffer {}", _handle);
}

// =============================================================================
void vkBuffer::destroy() {
    BTX_TRACE("Destroying buffer {}", _handle);
    _device.destroyBuffer(_handle);
    _handle = nullptr;
}

// =============================================================================
void vkBuffer::allocate(vk::MemoryPropertyFlags const flags) {
    if(_memory_handle) {
        BTX_CRITICAL("Device memory {} already allocated", _memory_handle);
        return;
    }

    // The first order of business is to query the logical device about what
    // available memory matches properties we've specified thus far. A zero-
    // initialized vk::MemoryRequirements structure indicates that the
    // allocation we're after is the whole size of the buffer we've already
    // described with no offset.
    vk::MemoryRequirements mem_reqs { };
    _device.getBufferMemoryRequirements(_handle, &mem_reqs);

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

    _memory_handle = _device.allocateMemory(alloc_info);
    BTX_TRACE("\n\tAllocated {} bytes: Device memory {}"
              "\n\tFor buffer {}",
              _size_bytes, _memory_handle, _handle);

    _device.bindBufferMemory(_handle, _memory_handle, 0u);
}

// =============================================================================
void vkBuffer::free() {
    BTX_TRACE("Freeing device memory {}", _memory_handle);
    _device.freeMemory(_memory_handle);
    _memory_handle = nullptr;
}

// =============================================================================
void vkBuffer::fill_buffer(void const *data) const {
    void *mapped_memory_handle;

    auto const &device = Renderer::device().native();
    auto const result = device.mapMemory(_memory_handle,
                                         0,
                                         VK_WHOLE_SIZE,
                                         { },
                                         &mapped_memory_handle);
    if(result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to map buffer {} memory {}: '{}'",
                     _handle, _memory_handle, vk::to_string(result));
    }

    ::memcpy(mapped_memory_handle, data, _size_bytes);

    device.unmapMemory(_memory_handle);
}

// =============================================================================
void vkBuffer::send_to_device(void const *data) const {
    vkBuffer staging_buffer;
    staging_buffer.create(_size_bytes, vk::BufferUsageFlagBits::eTransferSrc);
    staging_buffer.allocate((vk::MemoryPropertyFlagBits::eHostVisible |
                             vk::MemoryPropertyFlagBits::eHostCoherent));

    staging_buffer.fill_buffer(data);

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
    staging_buffer.free();
}

// =============================================================================
uint32_t vkBuffer::_get_memory_type_index(vk::MemoryPropertyFlags const flags,
                                          vk::MemoryRequirements const &reqs)
{
    auto const &device = *RenderConfig::current_device->device;
    auto const &memory_props = device.native().getMemoryProperties();
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