#ifndef BRASSTACKS_PLATFORM_VULKAN_RESOURCES_IMAGES_VKIMAGE_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RESOURCES_IMAGES_VKIMAGE_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDevice;
class vkPhysicalDevice;
class vkCmdBuffer;
class vkImageView;
class vkSampler;

class vkImage final {
public:
    // For referencing a prexisting image, such as a swapchain image
    vkImage(vkDevice const &device, vk::Image const &handle,
            vk::Format const format);

    // For reading texture data from a file
    vkImage(vkDevice const &device, std::string_view const filename,
            uint32_t const array_layers = 1u);

    ~vkImage();

    static void set_memory_props(
        vk::PhysicalDeviceMemoryProperties const &props
    );

    void create(vk::ImageType const type,
                vk::SampleCountFlagBits const samples,
                vk::ImageUsageFlags const usage_flags,
                vk::MemoryPropertyFlags const memory_flags);

    void create_sampler(vk::Filter const min_filter,
                        vk::Filter const mag_filter,
                        vk::SamplerMipmapMode const mip_filter,
                        vk::SamplerAddressMode const mode_u,
                        vk::SamplerAddressMode const mode_v);

    inline auto const & native()  const { return _handle;   }
    inline auto const & view()    const { return *_view;    }
    inline auto const & sampler() const { return *_sampler; }
    inline auto format()          const { return _format;   }
    inline auto layout()          const { return _layout;   }

    vkImage() = delete;

    vkImage(vkImage &&) = delete;
    vkImage(const vkImage &) = delete;

    vkImage & operator=(vkImage &&) = delete;
    vkImage & operator=(const vkImage &) = delete;

private:
    vkDevice const &_device;

    vk::Image        _handle;
    vk::DeviceMemory _memory;
    vkImageView     *_view;
    vkSampler       *_sampler;
    vk::Format       _format;
    vk::ImageLayout  _layout;
    vk::Extent3D     _extent;
    size_t           _size_bytes;
    uint32_t         _mip_levels;
    uint32_t         _array_layers;

    void *_raw_data;

    bool const _is_swapchain;

    static vk::PhysicalDeviceMemoryProperties _memory_props;

    void * _load_from_file(std::string_view const filename);

    void _calc_mip_levels();

    void _allocate(vk::MemoryPropertyFlags const memory_flags);

    uint32_t _find_memory_type(vk::MemoryPropertyFlags const flags,
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

#endif // BRASSTACKS_PLATFORM_VULKAN_RESOURCES_IMAGES_VKIMAGE_HPP