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
    _aspect_flags  { vk::ImageAspectFlagBits::eNone },
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

    if(_handle && _memory_handle) {
        destroy();
    }
}

// =============================================================================
vkImage::vkImage(vkImage &&rhs) :
    _handle        { rhs._handle },
    _memory_handle { rhs._memory_handle },
    _device        { rhs._device },
    _format        { rhs._format },
    _layout        { rhs._layout },
    _aspect_flags  { rhs._aspect_flags },
    _extent        { rhs._extent },
    _size_bytes    { rhs._size_bytes },
    _mip_levels    { rhs._mip_levels },
    _array_layers  { rhs._array_layers },
    _raw_data      { rhs._raw_data }
{
    rhs._handle        = nullptr;
    rhs._memory_handle = nullptr;
    rhs._device        = nullptr;
    rhs._format        = vk::Format::eUndefined;
    rhs._layout        = vk::ImageLayout::eUndefined;
    rhs._aspect_flags  = vk::ImageAspectFlagBits::eNone;
    rhs._extent        = { };
    rhs._size_bytes    = 0;
    rhs._mip_levels    = 1u;
    rhs._array_layers  = 1u;
    rhs._raw_data      = nullptr;
}

// =============================================================================
void vkImage::create(vk::Image const &handle, vk::Format const format) {
    if(_handle) {
        BTX_CRITICAL("Swapchain image {} already exists", _handle);
    }

    _handle = handle;
    _format = format;

    _extent = {
        .width = Renderer::swapchain().size().width,
        .height = Renderer::swapchain().size().height
    };

    _format = Renderer::swapchain().image_format();

    _aspect_flags = vk::ImageAspectFlagBits::eColor;
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
    _aspect_flags = image_info.aspect_flags;

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

    _aspect_flags = image_info.aspect_flags;

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
        .pNext                 = nullptr,
        .flags                 = { },
        .imageType             = image_info.type,
        .format                = _format,
        .extent                = _extent,
        .mipLevels             = _mip_levels,
        .arrayLayers           = _array_layers,
        .samples               = image_info.samples,
        .tiling                = vk::ImageTiling::eOptimal,
        .usage                 = image_info.usage_flags,
        .sharingMode           = vk::SharingMode::eExclusive,
        .queueFamilyIndexCount = 0u,
        .pQueueFamilyIndices   = nullptr,
        .initialLayout         = vk::ImageLayout::eUndefined,
    };

    _handle = _device.createImage(create_info);
    BTX_TRACE("Created image {}", _handle);

    _allocate(image_info.memory_flags);
    _send_to_device();
}

// =============================================================================
void vkImage::destroy() {
    if(_device) {
        BTX_TRACE("Destroying image {}", _handle);
        _device.destroyImage(_handle);

        BTX_TRACE("Freeing image device memory {}", _memory_handle);
        _device.freeMemory(_memory_handle);
    }

    if(_raw_data != nullptr) {
        BTX_WARN("Image {} being destroyed before raw data is freed!", _handle);
    }

    _handle        = nullptr;
    _memory_handle = nullptr;
    _device        = nullptr;
    _format        = vk::Format::eUndefined;
    _layout        = vk::ImageLayout::eUndefined;
    _aspect_flags  = vk::ImageAspectFlagBits::eNone;
    _extent        = { };
    _size_bytes    = 0;
    _mip_levels    = 1u;
    _array_layers  = 1u;
    _raw_data      = nullptr;
}

// =============================================================================
void vkImage::transition_layout(vkCmdBuffer const &cmd_buffer,
                                vk::ImageLayout const old_layout,
                                vk::ImageLayout const new_layout,
                                uint32_t const base_mip_level,
                                uint32_t const mip_level_count,
                                uint32_t const base_array_layer,
                                uint32_t const array_layer_count)
{
    // BTX_TRACE("Image {}: '{:s}'->'{:s}'",
    //           _handle,
    //           vk::to_string(old_layout),
    //           vk::to_string(new_layout));

    vk::ImageMemoryBarrier2KHR barrier {
        .pNext = nullptr,
        .srcStageMask  = vk::PipelineStageFlagBits2KHR::eAllCommands,
        .srcAccessMask = vk::AccessFlagBits2KHR::eMemoryWrite,
        .dstStageMask  = vk::PipelineStageFlagBits2KHR::eAllCommands,
        .dstAccessMask = vk::AccessFlagBits2KHR::eMemoryRead
                         | vk::AccessFlagBits2KHR::eMemoryWrite,
        .oldLayout = old_layout,
        .newLayout = new_layout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = _handle,
        .subresourceRange {
            .aspectMask     = _aspect_flags,
            .baseMipLevel   = base_mip_level,
            .levelCount     = mip_level_count,
            .baseArrayLayer = base_array_layer,
            .layerCount     = array_layer_count,
        }
    };

    if(old_layout == vk::ImageLayout::eUndefined) {
        if(new_layout == vk::ImageLayout::eColorAttachmentOptimal) {
            barrier.srcStageMask =
                vk::PipelineStageFlagBits2KHR::eColorAttachmentOutput;
            barrier.srcAccessMask =
                vk::AccessFlagBits2KHR::eColorAttachmentRead
                | vk::AccessFlagBits2KHR::eColorAttachmentWrite;

            barrier.dstStageMask =
                vk::PipelineStageFlagBits2KHR::eColorAttachmentOutput;
            barrier.dstAccessMask =
                vk::AccessFlagBits2KHR::eColorAttachmentRead
                | vk::AccessFlagBits2KHR::eColorAttachmentWrite;
        }
        else if(new_layout == vk::ImageLayout::eDepthAttachmentOptimal) {
            barrier.srcStageMask =
                vk::PipelineStageFlagBits2KHR::eEarlyFragmentTests
                | vk::PipelineStageFlagBits2KHR::eLateFragmentTests;

            barrier.dstStageMask =
                vk::PipelineStageFlagBits2KHR::eEarlyFragmentTests;
            barrier.dstAccessMask =
                vk::AccessFlagBits2KHR::eDepthStencilAttachmentRead
                | vk::AccessFlagBits2KHR::eDepthStencilAttachmentWrite;
        }
        else if(new_layout == vk::ImageLayout::eTransferDstOptimal) {
            barrier.srcStageMask = vk::PipelineStageFlagBits2KHR::eNone;

            barrier.dstStageMask = vk::PipelineStageFlagBits2KHR::eTransfer;
            barrier.dstAccessMask = vk::AccessFlagBits2KHR::eTransferWrite;
        }
        else {
            BTX_CRITICAL("Image {}: unsupported image layout transition: "
                         "'{:s}'->'{:s}'",
                         _handle,
                         vk::to_string(old_layout),
                         vk::to_string(new_layout));
            return;
        }
    }
    else if(old_layout == vk::ImageLayout::eColorAttachmentOptimal) {
        if(new_layout == vk::ImageLayout::ePresentSrcKHR) {
            barrier.srcStageMask =
                vk::PipelineStageFlagBits2KHR::eColorAttachmentOutput;
            barrier.srcAccessMask =
                vk::AccessFlagBits2KHR::eColorAttachmentRead
                | vk::AccessFlagBits2KHR::eColorAttachmentWrite;

            barrier.dstStageMask =
                vk::PipelineStageFlagBits2KHR::eColorAttachmentOutput;
            barrier.dstAccessMask = { };
        }
        else {
            BTX_CRITICAL("Image {}: unsupported swapchain image layout "
                         "transition: '{:s}'->'{:s}'",
                         _handle,
                         vk::to_string(old_layout),
                         vk::to_string(new_layout));
            return;
        }
    }
    else if(barrier.oldLayout == vk::ImageLayout::eTransferDstOptimal) {
        if(barrier.newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
            barrier.srcStageMask = vk::PipelineStageFlagBits2KHR::eTransfer;
            barrier.srcAccessMask = vk::AccessFlagBits2KHR::eTransferWrite;

            barrier.dstAccessMask = vk::AccessFlagBits2KHR::eShaderRead;
            barrier.dstStageMask = vk::PipelineStageFlagBits2KHR::eFragmentShader;
        }
        else if(barrier.newLayout == vk::ImageLayout::eTransferSrcOptimal) {
            barrier.srcStageMask = vk::PipelineStageFlagBits2KHR::eTransfer;
            barrier.srcAccessMask = vk::AccessFlagBits2KHR::eTransferWrite;

            barrier.dstAccessMask = vk::AccessFlagBits2KHR::eTransferRead;
            barrier.dstStageMask = vk::PipelineStageFlagBits2KHR::eTransfer;
        }
        else {
            BTX_CRITICAL("Image {}: unsupported image layout transition: "
                         "'{:s}'->'{:s}'",
                         _handle,
                         vk::to_string(old_layout),
                         vk::to_string(new_layout));
            return;
        }
    }
    else if(barrier.oldLayout == vk::ImageLayout::eTransferSrcOptimal) {
        if(barrier.newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
            barrier.srcStageMask = vk::PipelineStageFlagBits2KHR::eTransfer;
            barrier.srcAccessMask = vk::AccessFlagBits2KHR::eTransferRead;

            barrier.dstAccessMask = vk::AccessFlagBits2KHR::eShaderRead;
            barrier.dstStageMask = vk::PipelineStageFlagBits2KHR::eFragmentShader;
        }
        else {
            BTX_CRITICAL("Image {}: unsupported image layout transition: "
                         "'{:s}'->'{:s}'",
                         _handle,
                         vk::to_string(old_layout),
                         vk::to_string(new_layout));
            return;
        }
    }
    else {
        BTX_CRITICAL("Image {}: unsupported image layout transition: "
                     "'{:s}'->'{:s}'",
                     _handle,
                     vk::to_string(old_layout),
                     vk::to_string(new_layout));
        return;
    }

    auto dep_info = vk::DependencyInfoKHR {
        .pNext = nullptr,
        .dependencyFlags = { },
        .memoryBarrierCount = { },
        .pMemoryBarriers = { },
        .bufferMemoryBarrierCount = { },
        .pBufferMemoryBarriers = { },
        .imageMemoryBarrierCount = 1u,
        .pImageMemoryBarriers = &barrier,
    };

    cmd_buffer.native().pipelineBarrier2KHR(dep_info);

    _layout = barrier.newLayout;
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
            .aspectMask     = _aspect_flags,
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

        transition_layout(cmd_buffer,
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

            transition_layout(cmd_buffer,
                              vk::ImageLayout::eTransferDstOptimal,
                              vk::ImageLayout::eTransferSrcOptimal,
                              mip - 1u, 1u,
                              layer, 1u);

            vk::ImageBlit const blit {
                .srcSubresource {
                    .aspectMask     = _aspect_flags,
                    .mipLevel       = mip - 1,
                    .baseArrayLayer = layer,
                    .layerCount     = 1u,
                },
                .srcOffsets = std::array<vk::Offset3D, 2> {
                    vk::Offset3D { 0, 0, 0 },
                    vk::Offset3D { mip_width, mip_height, 1 }
                },
                .dstSubresource {
                    .aspectMask     = _aspect_flags,
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

            transition_layout(cmd_buffer,
                              vk::ImageLayout::eTransferSrcOptimal,
                              vk::ImageLayout::eShaderReadOnlyOptimal,
                              mip - 1u, 1u,
                              layer, 1u);

            if(mip_width  > 1) { mip_width  /= 2; }
            if(mip_height > 1) { mip_height /= 2; }
        }

        transition_layout(cmd_buffer,
                          vk::ImageLayout::eTransferDstOptimal,
                          vk::ImageLayout::eShaderReadOnlyOptimal,
                          _mip_levels - 1u, 1u,
                          layer, 1u);
    }
}

} // namespace btx