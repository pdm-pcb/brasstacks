#include "brasstacks/brasstacks.hpp"

#include "brasstacks/platform/vulkan/resources/vkSampler.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"

namespace btx {

vkSampler::vkSampler(vkDevice const &device,
                     vk::Filter const min_filter,
                     vk::Filter const mag_filter,
                     vk::SamplerMipmapMode const mip_filter,
                     vk::SamplerAddressMode const mode_u,
                     vk::SamplerAddressMode const mode_v) :
    _device { device }
{
    vk::SamplerCreateInfo const create_info {
        .magFilter        = mag_filter,
        .minFilter        = min_filter,
        .mipmapMode       = mip_filter,
        .addressModeU     = mode_u,
        .addressModeV     = mode_v,
        .mipLodBias       = 0.0f,
        .anisotropyEnable = VK_TRUE,
        .maxAnisotropy    = RenderConfig::anisotropy,
        .compareEnable    = VK_FALSE,
        .compareOp        = vk::CompareOp::eAlways,
        .minLod           = 0.0f,
        .maxLod           = VK_LOD_CLAMP_NONE,
        .borderColor      = vk::BorderColor::eIntOpaqueWhite,
        .unnormalizedCoordinates = VK_FALSE
    };

    auto const result = _device.native().createSampler(create_info);
    if(result.result != vk::Result::eSuccess) {
        BTX_CRITICAL("Failed to create image sampler.");
        return;
    }

    _handle = result.value;
    BTX_TRACE(
        "\nCreated image sampler {}"
        "\n\tMag Filter: {}"
        "\n\tMin Filter: {}"
        "\n\tMip Mode:   {}"
        "\n\tAddress U:  {}"
        "\n\tAddress V:  {}"
        "\n\tAnisotropy: {}",
        _handle,
        to_string(create_info.magFilter),
        to_string(create_info.minFilter),
        to_string(create_info.mipmapMode),
        to_string(create_info.addressModeU),
        to_string(create_info.addressModeV),
        create_info.maxAnisotropy
    );
}

// =============================================================================
vkSampler::~vkSampler() {
    BTX_TRACE("Destroying image sampler {}", _handle);

    _device.native().destroySampler(_handle);
}

} // namespace btx