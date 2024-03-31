#include "Demo.hpp"
#include "brasstacks/brasstacks.hpp"

#include "brasstacks/config/RenderConfig.hpp"
#include "brasstacks/passes/ColorDepthPass.hpp"

#include "brasstacks/tools/cameras/PerspectiveCamera.hpp"

#include "brasstacks/platform/vulkan/swapchain/vkSwapchain.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkPipeline.hpp"
#include "brasstacks/platform/vulkan/resources/vkImage.hpp"
#include "brasstacks/platform/vulkan/resources/vkImageView.hpp"
#include "brasstacks/platform/vulkan/resources/vkSampler.hpp"

#include "brasstacks/platform/vulkan/descriptors/vkDescriptorSetLayout.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorSet.hpp"

// =============================================================================
Demo::Demo() :
    Application("Demo"),
    _color_depth_pass   { nullptr },
    _plane_mesh         { },
    _plane_mat          { },
    _cube_mesh          { },
    _cube_mat           { },
    _texture            { nullptr },
    _texture_view       { nullptr },
    _texture_sampler    { nullptr },
    _texture_set_layout { nullptr },
    _texture_set        { nullptr }
{ }

// =============================================================================
void Demo::init() {
    _plane_mesh = btx::MeshLibrary::new_plane_mesh(
        {{
            { 1.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f },
            { 0.25f, 0.25f, 0.25f }
        }},
        0.75f
    );

    _cube_mesh = btx::MeshLibrary::new_cube_mesh(
        {{
            { 1.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f },
            { 0.25f, 0.25f, 0.25f },
            { 1.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f },
            { 0.5f, 0.5f, 0.5f },
        }},
        0.75f
    );

    _color_depth_pass = new btx::ColorDepthPass();

    _color_depth_pass->pipeline()
        .module_from_spirv("shaders/demo.vert",
                           vk::ShaderStageFlagBits::eVertex)
        .module_from_spirv("shaders/demo.frag",
                           vk::ShaderStageFlagBits::eFragment)
        .describe_vertex_input(btx::Vertex::bindings,
                               btx::Vertex::attributes)
        .add_descriptor_set_layout(btx::CameraController::camera_ubo_layout())
        .add_push_constant({ .stage_flags = vk::ShaderStageFlagBits::eVertex,
                             .size_bytes = sizeof(btx::math::Mat4) });

    _color_depth_pass->create_pipeline();
    _color_depth_pass->create_swapchain_resources();
}

// =============================================================================
void Demo::shutdown() {
    delete _color_depth_pass;
}

// =============================================================================
void Demo::update() {
    using namespace btx::math;

    _plane_mat = translate(Mat4::identity, -Vec3::unit_x * 1.25f) *
                 rotate(Mat4::identity,
                        20.0f * btx::TimeKeeper::app_run_time(),
                        Vec3::unit_z);

    _cube_mat = translate(Mat4::identity, Vec3::unit_x * 1.25f) *
                rotate(Mat4::identity,
                       10.0f * btx::TimeKeeper::app_run_time(),
                       Vec3::unit_y - Vec3::unit_x);
}

// =============================================================================
void Demo::record_commands() const {
    static std::array<btx::vkPipeline::PushConstant, 1> const plane_pcs {
        btx::vkPipeline::PushConstant {
            .stage_flags = vk::ShaderStageFlagBits::eVertex,
            .size_bytes = sizeof(btx::math::Mat4),
            .data = &_plane_mat,
        }
    };

    static std::array<btx::vkPipeline::PushConstant, 1> const cube_pcs {
        btx::vkPipeline::PushConstant {
            .stage_flags = vk::ShaderStageFlagBits::eVertex,
            .size_bytes = sizeof(btx::math::Mat4),
            .data = &_cube_mat,
        }
    };

    _color_depth_pass->begin();

        _color_depth_pass->bind_descriptor_set(btx::CameraController::camera_ubo_set());

        _color_depth_pass->send_push_constants(plane_pcs);
        (*_plane_mesh)->draw_indexed(btx::Renderer::cmd_buffer());

        _color_depth_pass->send_push_constants(cube_pcs);
        (*_cube_mesh)->draw_indexed(btx::Renderer::cmd_buffer());

    _color_depth_pass->end();
}

// =============================================================================
void Demo::destroy_swapchain_resources() {
    _color_depth_pass->destroy_swapchain_resources();
}

// =============================================================================
void Demo::create_swapchain_resources() {
    _color_depth_pass->create_swapchain_resources();
}