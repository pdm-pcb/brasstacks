#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/vulkan/resources/vkSampler.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/config/RenderConfig.hpp"

namespace btx {

// =============================================================================
vkSampler::vkSampler() :
    _handle { nullptr },
    _device { nullptr }
{ }

// =============================================================================
vkSampler::~vkSampler() {
    if(_handle) {
        destroy();
    }
}

// =============================================================================
void vkSampler::create(vk::Filter const min_filter,
                       vk::Filter const mag_filter,
                       vk::SamplerMipmapMode const mip_filter,
                       vk::SamplerAddressMode const mode_u,
                       vk::SamplerAddressMode const mode_v)
{
    if(_handle) {
        BTX_CRITICAL("Sampler {} already exists", _handle);
        return;
    }

    _device = Renderer::device().native();

    vk::SamplerCreateInfo const create_info {
        .magFilter        = mag_filter,
        .minFilter        = min_filter,
        .mipmapMode       = mip_filter,
        .addressModeU     = mode_u,
        .addressModeV     = mode_v,
        .mipLodBias       = 0.0f,
        .anisotropyEnable = VK_TRUE,
        .maxAnisotropy    =
            static_cast<float>(RenderConfig::current_aniso->aniso),
        .compareEnable    = VK_FALSE,
        .compareOp        = vk::CompareOp::eAlways,
        .minLod           = 0.0f,
        .maxLod           = VK_LOD_CLAMP_NONE,
        .borderColor      = vk::BorderColor::eIntOpaqueWhite,
        .unnormalizedCoordinates = VK_FALSE
    };

    _handle = _device.createSampler(create_info);
    BTX_TRACE(
        "\nCreated image sampler {}"
        "\n\tMag Filter: {}"
        "\n\tMin Filter: {}"
        "\n\tMip Mode:   {}"
        "\n\tAddress U:  {}"
        "\n\tAddress V:  {}"
        "\n\tAnisotropy: {}",
        _handle,
        vk::to_string(create_info.magFilter),
        vk::to_string(create_info.minFilter),
        vk::to_string(create_info.mipmapMode),
        vk::to_string(create_info.addressModeU),
        vk::to_string(create_info.addressModeV),
        create_info.maxAnisotropy
    );
}

// =============================================================================
void vkSampler::destroy() {
    BTX_TRACE("Destroying image sampler {}", _handle);
    _device.destroySampler(_handle);
    _handle = nullptr;
}

} // namespace btx