#include "brasstacks/brasstacks.hpp"
#include "brasstacks/assets/textures/Texture.hpp"

namespace btx {

// =============================================================================
Texture::Texture(std::string_view const filepath) :
    _texture { std::make_unique<vkImage>() },
    _view    { std::make_unique<vkImageView>() },
    _sampler { std::make_unique<vkSampler>() },
    _descriptor_set_layout { std::make_unique<vkDescriptorSetLayout>() },
    _descriptor_set        { std::make_unique<vkDescriptorSet>() }
{
    _texture->create(
        filepath,
        btx::vkImage::ImageInfo {
            .type= vk::ImageType::e2D,
            .samples = vk::SampleCountFlagBits::e1,
            .usage_flags = (vk::ImageUsageFlagBits::eSampled |
                            vk::ImageUsageFlagBits::eTransferDst |
                            vk::ImageUsageFlagBits::eTransferSrc),
            .memory_flags = vk::MemoryPropertyFlagBits::eDeviceLocal
        }
    );

    _view->create(_texture->native(),
                  _texture->format(),
                  vk::ImageViewType::e2D,
                  vk::ImageAspectFlagBits::eColor);

    _sampler->create(vk::Filter::eLinear,
                     vk::Filter::eLinear,
                     vk::SamplerMipmapMode::eLinear,
                     vk::SamplerAddressMode::eRepeat,
                     vk::SamplerAddressMode::eRepeat);

    (*_descriptor_set_layout)
        .add_binding(vk::DescriptorType::eCombinedImageSampler,
                     vk::ShaderStageFlagBits::eFragment)
        .create();

    _descriptor_set->allocate(btx::Renderer::descriptor_pool(),
                              *_descriptor_set_layout);

    update_descriptor_set();
}

// =============================================================================
Texture::~Texture() {
    _texture->destroy();
    _view->destroy();
    _sampler->destroy();
    _descriptor_set_layout->destroy();
}

// =============================================================================
void Texture::update_sampler() {
        _sampler->destroy();
        _sampler->create(vk::Filter::eLinear,
                         vk::Filter::eLinear,
                         vk::SamplerMipmapMode::eLinear,
                         vk::SamplerAddressMode::eRepeat,
                         vk::SamplerAddressMode::eRepeat);
}

// =============================================================================
void Texture::update_descriptor_set() {
    (*_descriptor_set)
        .add_image(*_texture, *_view, *_sampler,
                   vk::DescriptorType::eCombinedImageSampler)
        .update_set();
}

} // namespace btx