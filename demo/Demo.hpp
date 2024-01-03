#ifndef DEMO_HPP
#define DEMO_HPP

#include "brasstacks/system/Application.hpp"
#include "brasstacks/math/math.hpp"

namespace btx {

struct WindowSizeEvent;

class vkDevice;

class vkColorPass;
class vkPipeline;
class vkFramebuffer;

class vkDescriptorPool;
class vkDescriptorSetLayout;
class vkDescriptorSet;
class vkBuffer;
class vkImage;
class vkImageView;
class vkSampler;

class FPSCamera;
class PlaneMesh;
class CubeMesh;

} // namespace btx

class Demo final : public btx::Application {
public:
    void init(btx::vkDevice const &device,
              btx::vkSwapchain const &swapchain) override;
    void shutdown() override;

    void update() override;
    void record_commands(btx::vkCmdBuffer const &cmd_buffer,
                         uint32_t const image_index) override;

    void create_framebuffers(btx::vkDevice const &device,
                             btx::vkSwapchain const &swapchain) override;
    void destroy_framebuffers() override;

    Demo();
    ~Demo() override = default;

private:
    struct PushConstant {
        vk::ShaderStageFlags const stage_flags = vk::ShaderStageFlagBits::eAll;
        size_t               const size_bytes  = 0;
        void                 const *data       = nullptr;
    };

    btx::vkDescriptorPool *_descriptor_pool;

    btx::PlaneMesh *_plane_mesh;
    btx::math::Mat4 _plane_mat;

    btx::CubeMesh  *_cube_mesh;
    btx::math::Mat4 _cube_mat;

    btx::vkImage     *_texture;
    btx::vkImageView *_texture_view;
    btx::vkSampler   *_texture_sampler;

    btx::vkDescriptorSetLayout *_texture_set_layout;
    btx::vkDescriptorSet       *_texture_set;

    btx::FPSCamera                     *_camera;
    std::vector<btx::vkBuffer *>        _camera_ubos;
    btx::vkDescriptorSetLayout         *_camera_ubo_layout;
    std::vector<btx::vkDescriptorSet *> _camera_ubo_sets;

    btx::vkColorPass                 *_color_pass;
    btx::vkPipeline                  *_color_pipeline;
    std::vector<btx::vkFramebuffer *> _color_framebuffers;

    void _create_camera(btx::vkDevice const &device);
    void _destroy_camera();

    void _create_texture(btx::vkDevice const &device);
    void _destroy_texture();

    void _create_color_pass(btx::vkDevice const &device,
                            btx::vkSwapchain const &swapchain);

    void _destroy_color_pass();

    void _record_color_commands(btx::vkCmdBuffer const &cmd_buffer,
                                uint32_t const image_index);

    using PushConstants = std::vector<PushConstant>;
    void _send_color_push_constants(btx::vkCmdBuffer const &cmd_buffer,
                                    PushConstants const &push_constants);
};

#endif // DEMO_HPP