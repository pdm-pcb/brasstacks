#ifndef DEMO_HPP
#define DEMO_HPP

#include "brasstacks/system/Application.hpp"
#include "brasstacks/math/math.hpp"

namespace btx {

class vkDevice;

class RenderPass;
class vkRenderPass;
class vkPipeline;
class vkFramebuffer;

class vkDescriptorPool;
class vkDescriptorSetLayout;
class vkDescriptorSet;
class FPSCamera;
class vkBuffer;
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

    Demo();
    ~Demo() override;

private:
    struct PushConstant {
        vk::ShaderStageFlags const stage_flags = vk::ShaderStageFlagBits::eAll;
        size_t               const size_bytes  = 0;
        void                 const *data       = nullptr;
    };

    btx::vkDescriptorPool *_descriptor_pool;

    btx::vkRenderPass                *_render_pass;
    btx::vkPipeline                  *_pipeline;
    std::vector<btx::vkFramebuffer *> _framebuffers;

    btx::CubeMesh  *_cube_mesh;
    btx::math::Mat4 _cube_mat;

    btx::FPSCamera                     *_camera;
    btx::vkDescriptorSetLayout         *_camera_ubo_layout;
    std::vector<btx::vkDescriptorSet *> _camera_ubo_sets;
    std::vector<btx::vkBuffer *>        _camera_ubos;

    void _create_camera_resources(btx::vkDevice const &device);
    void _destroy_camera_resources();

    void _create_render_pass(btx::vkDevice const &device,
                             btx::vkSwapchain const &swapchain);
    void _destroy_render_pass();

    using PushConstants = std::vector<PushConstant>;
    void _send_push_constants(btx::vkCmdBuffer const &cmd_buffer,
                              PushConstants const &push_constants);
};

#endif // DEMO_HPP