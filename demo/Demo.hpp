#ifndef DEMO_HPP
#define DEMO_HPP

#include "brasstacks/core/Application.hpp"

#include "brasstacks/math/Mat4.hpp"
#include "brasstacks/assets/libraries/MeshLibrary.hpp"

namespace btx {
    class ColorDepthPass;

    class PerspectiveCamera;

    class vkImage;
    class vkImageView;
    class vkSampler;

    class vkBuffer;

    class vkDescriptorPool;
    class vkDescriptorSetLayout;
    class vkDescriptorSet;
} // namespace btx

class Demo final : public btx::Application {
public:
    Demo();
    ~Demo() override = default;

    void init(btx::Renderer const &renderer) override;
    void shutdown() override;

    void update() override;
    void record_commands() const override;

    void destroy_swapchain_resources() override;
    void recreate_swapchain_resources() override;

private:
    btx::Renderer const *_renderer;

    btx::ColorDepthPass *_color_depth_pass;

    btx::MeshLibrary::MeshIter _plane_mesh;
    btx::math::Mat4            _plane_mat;

    btx::MeshLibrary::MeshIter _cube_mesh;
    btx::math::Mat4            _cube_mat;

    btx::vkDescriptorPool *_descriptor_pool;

    btx::vkImage     *_texture;
    btx::vkImageView *_texture_view;
    btx::vkSampler   *_texture_sampler;

    btx::vkDescriptorSetLayout *_texture_set_layout;
    btx::vkDescriptorSet       *_texture_set;

    btx::PerspectiveCamera             *_camera;
    std::vector<btx::vkBuffer *>        _camera_ubos;
    btx::vkDescriptorSetLayout         *_camera_ubo_layout;
    std::vector<btx::vkDescriptorSet *> _camera_ubo_sets;

    void _create_camera();
    void _destroy_camera();
};

#endif // DEMO_HPP