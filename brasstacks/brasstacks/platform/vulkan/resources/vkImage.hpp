#ifndef BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKIMAGE_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKIMAGE_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkCmdBuffer;

class vkImage final {
public:
    vkImage();
    ~vkImage();

    struct ImageInfo final {
        vk::ImageType type = { };
        vk::SampleCountFlagBits samples = { };
        vk::ImageUsageFlags usage_flags = { };
        vk::MemoryPropertyFlags memory_flags = { };
        vk::ImageAspectFlags aspect_flags = { };
    };

    // For render targets, eg color buffer
    void create(vk::Extent2D const &extent, vk::Format const format,
                ImageInfo const &image_info);

    // For reading texture data from a file
    void create(std::string_view const filename, ImageInfo const &image_info,
                uint32_t const array_layers = 1u);

    void transition_layout(vkCmdBuffer const &cmd_buffer,
                           vk::ImageLayout const old_layout,
                           vk::ImageLayout const new_layout);

    void destroy();

    inline auto const & native() const { return _handle; }
    inline auto format()         const { return _format; }
    inline auto layout()         const { return _layout; }

    vkImage(vkImage &&rhs);
    vkImage(const vkImage &) = delete;

    vkImage & operator=(vkImage &&) = delete;
    vkImage & operator=(const vkImage &) = delete;

private:
    vk::Image        _handle;
    vk::DeviceMemory _memory_handle;

    vk::Device _device;

    vk::Format           _format;
    vk::ImageLayout      _layout;
    vk::ImageAspectFlags _aspect_flags;

    vk::Extent3D    _extent;
    size_t          _size_bytes;
    uint32_t        _mip_levels;
    uint32_t        _array_layers;

    void *_raw_data;

    void * _load_from_file(std::string_view const filename);

    void _calc_mip_levels();

    void _allocate(vk::MemoryPropertyFlags const memory_flags);

    static uint32_t _memory_type_index(vk::MemoryPropertyFlags const flags,
                                       vk::MemoryRequirements const reqs);

    void _send_to_device();

    void _generate_mipmaps(vkCmdBuffer const &cmd_buffer,
                           vk::Filter const filter);

    void _transition_layout(vkCmdBuffer const &cmd_buffer,
                            vk::ImageLayout const old_layout,
                            vk::ImageLayout const new_layout,
                            uint32_t const base_mip_level,
                            uint32_t const mip_level_count,
                            uint32_t const base_array_layer,
                            uint32_t const array_layer_count);
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKIMAGE_HPP