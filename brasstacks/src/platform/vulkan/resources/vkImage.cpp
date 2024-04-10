#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/resources/vkImage.hpp"

#include "brasstacks/platform/vulkan/devices/vkPhysicalDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"
#include "brasstacks/platform/vulkan/resources/vkBuffer.hpp"

#include <stb/stb_image.h>

namespace btx {

// =============================================================================
vkImage::vkImage() :
    _handle        { nullptr },
    _memory_handle { nullptr },
    _device        { nullptr },
    _format        { vk::Format::eUndefined },
    _layout        { vk::ImageLayout::eUndefined },
    _extent        { },
    _size_bytes    { 0 },
    _mip_levels    { 1u },
    _array_layers  { 1u },
    _raw_data      { nullptr }
{ }

// =============================================================================
vkImage::~vkImage() {
    if(_raw_data != nullptr) {
        ::stbi_image_free(_raw_data);
    }

    if(_handle || _memory_handle) {
        destroy();
    }
}

// =============================================================================
void vkImage::create(vk::Extent2D const &extent, vk::Format const format,
                     ImageInfo const &image_info)
{
    if(_handle) {
        BTX_CRITICAL("Image {} already exists", _handle);
        return;
    }

    _device = Renderer::device().native();

    _format = format;
    _extent = {
        .width = extent.width,
        .height = extent.height,
        .depth = 1u
    };

    vk::ImageCreateInfo const create_info {
        .pNext = nullptr,
        .flags = { },
        .imageType = image_info.type,
        .format = _format,
        .extent = _extent,
        .mipLevels = _mip_levels,
        .arrayLayers = _array_layers,
        .samples = image_info.samples,
        .tiling = vk::ImageTiling::eOptimal,
        .usage = image_info.usage_flags,
        .sharingMode = vk::SharingMode::eExclusive,
        .queueFamilyIndexCount = 0u,
        .pQueueFamilyIndices = nullptr,
        .initialLayout = vk::ImageLayout::eUndefined,
    };

    _handle = _device.createImage(create_info);
    BTX_TRACE("Created image {} with extent {}x{}, samples {}", _handle,
              extent.width, extent.height, vk::to_string(image_info.samples));

    _allocate(image_info.memory_flags);
}

// =============================================================================
void vkImage::create(std::string_view const filename,
                     ImageInfo const &image_info,
                     uint32_t const array_layers)
{
    if(_handle) {
        BTX_CRITICAL("Image {} already exists", _handle);
        return;
    }

    _device = Renderer::device().native();

    _array_layers = array_layers;

    _raw_data = _load_from_file(filename);
    _calc_mip_levels();

    BTX_TRACE(
        "\nCreating image with"
        "\n\tSize:         {} bytes"
        "\n\tExtent:       {}x{}x{}"
        "\n\tMip Levels:   {}"
        "\n\tArray Layers: {}",
        _size_bytes,
        _extent.width, _extent.height, _extent.depth,
        _mip_levels,
        _array_layers
    );

    vk::ImageCreateInfo const create_info {
        .pNext = nullptr,
        .flags = { },
        .imageType = image_info.type,
        .format = _format,
        .extent = _extent,
        .mipLevels = _mip_levels,
        .arrayLayers = _array_layers,
        .samples = image_info.samples,
        .tiling = vk::ImageTiling::eOptimal,
        .usage = image_info.usage_flags,
        .sharingMode = vk::SharingMode::eExclusive,
        .queueFamilyIndexCount = 0u,
        .pQueueFamilyIndices = nullptr,
        .initialLayout = vk::ImageLayout::eUndefined,
    };

    _handle = _device.createImage(create_info);
    BTX_TRACE("Created image {}", _handle);

    _allocate(image_info.memory_flags);
    _send_to_device();
}

// =============================================================================
void vkImage::destroy() {
    BTX_TRACE("Destroying image {}", _handle);
    _device.destroyImage(_handle);
    _handle = nullptr;

    BTX_TRACE("Freeing image device memory {}", _memory_handle);
    _device.freeMemory(_memory_handle);
    _memory_handle = nullptr;
}

// =============================================================================
void * vkImage::_load_from_file(std::string_view const filename) {
    auto const filepath = BTX_ASSET_PATH / filename;

    int width = 0;
    int height = 0;
    int channels = 0;

    ::stbi_uc *data = ::stbi_load(
        filepath.string().c_str(),
        &width, &height, &channels,
        ::STBI_rgb_alpha
    );

    if(data == nullptr) {
        BTX_CRITICAL("Failed to load image '{}'"
                     "\n\t{}x{} @ {}bpc"
                     "\n\tError: '{}'",
                     filepath.string(), width, height, channels,
                     ::stbi_failure_reason());
    }
    else {
        BTX_TRACE("Loaded image {}", filepath.string());

        _extent.width  = static_cast<uint32_t>(width);
        _extent.height = static_cast<uint32_t>(height);
        _extent.depth  = 1u;

        channels = static_cast<uint32_t>(::STBI_rgb_alpha);
        _size_bytes = _extent.width * _extent.height *
                      static_cast<uint32_t>(channels);
        _format = vk::Format::eR8G8B8A8Unorm;
    }

    return data;
}

// =============================================================================
void vkImage::_calc_mip_levels() {
    auto const longest_side = std::max(static_cast<float>(_extent.width),
                                       static_cast<float>(_extent.height));

    _mip_levels =
        static_cast<uint32_t>(std::floor(std::log2(longest_side))) + 1u;
}

// =============================================================================
void vkImage::_allocate(vk::MemoryPropertyFlags const memory_flags) {
    vk::MemoryRequirements mem_reqs { };

    _device.getImageMemoryRequirements(_handle, &mem_reqs);

    auto type_index = _memory_type_index(memory_flags, mem_reqs);

    vk::MemoryAllocateInfo const alloc_info {
        .allocationSize  = mem_reqs.size,
        .memoryTypeIndex = type_index,
    };

    _memory_handle = _device.allocateMemory(alloc_info);
    BTX_TRACE("Allocated {} bytes {} for image {}",
              mem_reqs.size, _memory_handle, _handle);

    _device.bindImageMemory(_handle, _memory_handle, 0u);
}

// =============================================================================
uint32_t vkImage::_memory_type_index(vk::MemoryPropertyFlags const flags,
                                     vk::MemoryRequirements const reqs)
{
    auto const &memory_props = RenderConfig::current_device->memory;
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

    BTX_CRITICAL("Could not find memory to match image requirements.");
    return std::numeric_limits<uint32_t>::max();
}

// =============================================================================
void vkImage::_send_to_device() {
    vkBuffer staging_buffer;
    staging_buffer.create(_size_bytes,
                          vk::BufferUsageFlagBits::eTransferSrc);
    staging_buffer.allocate((vk::MemoryPropertyFlagBits::eHostVisible |
                             vk::MemoryPropertyFlagBits::eHostCoherent));

    staging_buffer.fill_buffer(_raw_data);

    vk::BufferImageCopy const copy_region {
        .bufferOffset      = 0u,
        .bufferRowLength   = 0u,
        .bufferImageHeight = 0u,
        .imageSubresource {
            .aspectMask     = vk::ImageAspectFlagBits::eColor,
            .mipLevel       = 0u,
            .baseArrayLayer = 0u,
            .layerCount     = _array_layers,
        },
        .imageOffset {
            .x = 0,
            .y = 0,
            .z = 0
        },
        .imageExtent = _extent
    };

    vkCmdBuffer cmd_buffer;
    cmd_buffer.allocate(Renderer::device().transient_pool());
    cmd_buffer.begin_one_time_submit();

        _transition_layout(cmd_buffer,
                           vk::ImageLayout::eUndefined,
                           vk::ImageLayout::eTransferDstOptimal,
                           0u, _mip_levels,
                           0u, _array_layers);

        cmd_buffer.native().copyBufferToImage(
            staging_buffer.native(),
            _handle,
            _layout,
            copy_region
        );

        _generate_mipmaps(cmd_buffer, vk::Filter::eLinear);

    cmd_buffer.end_recording();
    cmd_buffer.submit_and_wait_on_device();
    cmd_buffer.free();

    ::stbi_image_free(_raw_data);
    _raw_data = nullptr;
}

// =============================================================================
void vkImage::_generate_mipmaps(vkCmdBuffer const &cmd_buffer,
                                vk::Filter const filter)
{
    for(uint32_t layer = 0u; layer < _array_layers; ++layer) {
        BTX_TRACE("Processing array layer {}", layer);

        int32_t mip_width  = static_cast<int32_t>(_extent.width);
        int32_t mip_height = static_cast<int32_t>(_extent.height);

        for(uint32_t mip = 1u; mip < _mip_levels; ++mip) {
            BTX_TRACE("Generating mip level {}", mip);

            _transition_layout(cmd_buffer,
                               vk::ImageLayout::eTransferDstOptimal,
                               vk::ImageLayout::eTransferSrcOptimal,
                               mip - 1u, 1u,
                               layer, 1u);

            vk::ImageBlit const blit {
                .srcSubresource {
                    .aspectMask     = vk::ImageAspectFlagBits::eColor,
                    .mipLevel       = mip - 1,
                    .baseArrayLayer = layer,
                    .layerCount     = 1u,
                },
                .srcOffsets = std::array<vk::Offset3D, 2> {
                    vk::Offset3D { 0, 0, 0 },
                    vk::Offset3D { mip_width, mip_height, 1 }
                },
                .dstSubresource {
                    .aspectMask     = vk::ImageAspectFlagBits::eColor,
                    .mipLevel       = mip,
                    .baseArrayLayer = layer,
                    .layerCount     = 1u,
                },
                .dstOffsets = std::array<vk::Offset3D, 2> {
                    vk::Offset3D { 0, 0, 0 },
                    vk::Offset3D {
                        (mip_width  > 1 ? mip_width  / 2 : 1),
                        (mip_height > 1 ? mip_height / 2 : 1),
                        1
                    }
                },
            };

            cmd_buffer.native().blitImage(
                _handle, vk::ImageLayout::eTransferSrcOptimal,
                _handle, vk::ImageLayout::eTransferDstOptimal,
                { blit },
                filter
            );

            _transition_layout(cmd_buffer,
                               vk::ImageLayout::eTransferSrcOptimal,
                               vk::ImageLayout::eShaderReadOnlyOptimal,
                               mip - 1u, 1u,
                               layer, 1u);

            if(mip_width  > 1) { mip_width  /= 2; }
            if(mip_height > 1) { mip_height /= 2; }
        }

        _transition_layout(cmd_buffer,
                           vk::ImageLayout::eTransferDstOptimal,
                           vk::ImageLayout::eShaderReadOnlyOptimal,
                           _mip_levels - 1u, 1u,
                           layer, 1u);
    }
}

// =============================================================================
void vkImage::_transition_layout(vkCmdBuffer const &cmd_buffer,
                                 vk::ImageLayout const old_layout,
                                 vk::ImageLayout const new_layout,
                                 uint32_t const base_mip_level,
                                 uint32_t const mip_level_count,
                                 uint32_t const base_array_layer,
                                 uint32_t const array_layer_count)
{
    vk::ImageMemoryBarrier barrier {
        .oldLayout = old_layout,
        .newLayout = new_layout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = _handle,
        .subresourceRange {
            .aspectMask     = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel   = base_mip_level,
            .levelCount     = mip_level_count,
            .baseArrayLayer = base_array_layer,
            .layerCount     = array_layer_count,
        }
    };

    BTX_TRACE(
        "Image {}, mip {} ({}), layer {} ({}): '{:s}' -> '{:s}'",
        _handle,
        base_mip_level, _mip_levels,
        base_array_layer, _array_layers,
        vk::to_string(barrier.oldLayout),
        vk::to_string(barrier.newLayout)
    );

    vk::PipelineStageFlags src_stage = vk::PipelineStageFlagBits::eNone;
    vk::PipelineStageFlags dst_stage = vk::PipelineStageFlagBits::eNone;

    if(barrier.oldLayout == vk::ImageLayout::eUndefined) {
        if(new_layout == vk::ImageLayout::eTransferDstOptimal) {
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

            src_stage = vk::PipelineStageFlagBits::eTopOfPipe;
            dst_stage = vk::PipelineStageFlagBits::eTransfer;
        }
        else {
            BTX_CRITICAL("Unsupported image layout transition");
            return;
        }
    }
    else if(barrier.oldLayout == vk::ImageLayout::eTransferDstOptimal) {
        if(barrier.newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            src_stage      = vk::PipelineStageFlagBits::eTransfer;
            dst_stage = vk::PipelineStageFlagBits::eFragmentShader;
        }
        else if(barrier.newLayout == vk::ImageLayout::eTransferSrcOptimal) {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

            src_stage = vk::PipelineStageFlagBits::eTransfer;
            dst_stage = vk::PipelineStageFlagBits::eTransfer;
        }
        else {
            BTX_CRITICAL("Unsupported image layout transition");
            return;
        }
    }
    else if(barrier.oldLayout == vk::ImageLayout::eTransferSrcOptimal) {
        if(barrier.newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            src_stage = vk::PipelineStageFlagBits::eTransfer;
            dst_stage = vk::PipelineStageFlagBits::eFragmentShader;
        }
        else {
            BTX_CRITICAL("Unsupported image layout transition");
            return;
        }
    }
    else {
        BTX_CRITICAL("Unsupported image layout transition");
        return;
    }

    cmd_buffer.native().pipelineBarrier(
        src_stage,  // Source stage
        dst_stage,  // Destination stage
        { },        // Dependency flags
        nullptr,    // Memory barriers
        nullptr,    // Buffer memory barriers
        { barrier } // Image memory barriers
    );

    _layout = barrier.newLayout;
}

} // namespace btx