#ifndef BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKSAMPLER_HPP
#define BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKSAMPLER_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class vkDevice;

class vkSampler final {
public:
    vkSampler(vkDevice const &device,
              vk::Filter const min_filter,
              vk::Filter const mag_filter,
              vk::SamplerMipmapMode const mip_filter,
              vk::SamplerAddressMode const mode_u,
              vk::SamplerAddressMode const mode_v);

    ~vkSampler();

    auto const & native() const { return _handle; }

    vkSampler() = delete;

    vkSampler(vkSampler &&) = delete;
    vkSampler(vkSampler const &) = delete;

    vkSampler & operator=(vkSampler &&) = delete;
    vkSampler & operator=(vkSampler const &) = delete;

private:
    vkDevice const &_device;
    vk::Sampler _handle;
};

} // namespace btx

#endif // BRASSTACKS_PLATFORM_VULKAN_RESOURCES_VKSAMPLER_HPP