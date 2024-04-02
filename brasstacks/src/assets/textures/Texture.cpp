#include "brasstacks/brasstacks.hpp"
#include "brasstacks/assets/textures/Texture.hpp"

namespace btx {

// =============================================================================
Texture::Texture() :
    _texture               { std::make_unique<vkImage>() },
    _texture_view          { std::make_unique<vkImageView>() },
    _texture_sampler       { std::make_unique<vkSampler>() },
    _descriptor_set_layout { std::make_unique<vkDescriptorSetLayout>() },
    _descriptor_set        { std::make_unique<vkDescriptorSet>() }
{ }

// =============================================================================
Texture::~Texture() {
    _texture->destroy();
    _texture_view->destroy();
    _texture_sampler->destroy();
    _descriptor_set_layout->destroy();
}

// =============================================================================
void Texture::create(std::string_view const filepath) {
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

    _texture_view->create(_texture->native(),
                          _texture->format(),
                          vk::ImageViewType::e2D,
                          vk::ImageAspectFlagBits::eColor);

    _texture_sampler->create(vk::Filter::eLinear,
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

    (*_descriptor_set)
        .add_image(*_texture, *_texture_view, *_texture_sampler,
                   vk::DescriptorType::eCombinedImageSampler)
        .write_set();
}

} // namespace btx