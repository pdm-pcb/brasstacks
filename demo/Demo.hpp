#ifndef DEMO_HPP
#define DEMO_HPP

#include "brasstacks/core/Application.hpp"

#include "brasstacks/math/Mat4.hpp"
#include "brasstacks/assets/libraries/MeshLibrary.hpp"
#include "brasstacks/passes/ColorDepthPass.hpp"

namespace btx {
    class vkImage;
    class vkImageView;
    class vkSampler;

    class vkDescriptorSetLayout;
    class vkDescriptorSet;
} // namespace btx

class Demo final : public btx::Application {
public:
    Demo();
    ~Demo() override = default;

    void init() override;
    void shutdown() override;

    void update() override;
    void record_commands() const override;

    void destroy_swapchain_resources() override;
    void create_swapchain_resources() override;

private:
    std::unique_ptr<btx::ColorDepthPass> _color_depth_pass;

    btx::MeshLibrary::MeshIter _plane_mesh;
    btx::math::Mat4            _plane_mat;

    btx::MeshLibrary::MeshIter _cube_mesh;
    btx::math::Mat4            _cube_mat;

    btx::vkImage     *_texture;
    btx::vkImageView *_texture_view;
    btx::vkSampler   *_texture_sampler;

    btx::vkDescriptorSetLayout *_texture_set_layout;
    btx::vkDescriptorSet       *_texture_set;
};

#endif // DEMO_HPP