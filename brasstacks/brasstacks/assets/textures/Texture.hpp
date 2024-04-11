#ifndef BRASSTACKS_ASSETS_TEXTURES_TEXTURE_HPP
#define BRASSTACKS_ASSETS_TEXTURES_TEXTURE_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/platform/vulkan/resources/vkImage.hpp"
#include "brasstacks/platform/vulkan/resources/vkImageView.hpp"
#include "brasstacks/platform/vulkan/resources/vkSampler.hpp"

#include "brasstacks/platform/vulkan/descriptors/vkDescriptorSetLayout.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorSet.hpp"

namespace btx {

class Texture final {
public:
    explicit Texture(std::string_view const filepath);
    ~Texture();

    void update_sampler();
    void update_descriptor_set();

    auto const & sampler() const { return _sampler; }

    auto const & descriptor_set_layout() const {
        return _descriptor_set_layout;
    }

    auto const & descriptor_set() const { return _descriptor_set; }

    Texture() = delete;

    Texture(Texture &&) = delete;
    Texture(Texture const &) = delete;

    Texture & operator=(Texture &&) = delete;
    Texture & operator=(Texture const &) = delete;

private:
    vkImage     _texture;
    vkImageView _view;
    vkSampler   _sampler;

    vkDescriptorSetLayout _descriptor_set_layout;
    vkDescriptorSet       _descriptor_set;
};

} // namespace btx

#endif // BRASSTACKS_ASSETS_TEXTURES_TEXTURE_HPP