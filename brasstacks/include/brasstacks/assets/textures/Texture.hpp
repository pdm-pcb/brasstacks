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
    Texture();
    ~Texture();

    void create(std::string_view const filepath);

    auto const & descriptor_set_layout() const {
        return *_descriptor_set_layout;
    }

    auto const & descriptor_set() const { return *_descriptor_set; }

    Texture(Texture &&) = delete;
    Texture(Texture const &) = delete;

    Texture & operator=(Texture &&) = delete;
    Texture & operator=(Texture const &) = delete;

private:
    std::unique_ptr<vkImage>     _texture;
    std::unique_ptr<vkImageView> _texture_view;
    std::unique_ptr<vkSampler>   _texture_sampler;

    std::unique_ptr<vkDescriptorSetLayout> _descriptor_set_layout;
    std::unique_ptr<vkDescriptorSet>       _descriptor_set;
};

} // namespace btx

#endif // BRASSTACKS_ASSETS_TEXTURES_TEXTURE_HPP