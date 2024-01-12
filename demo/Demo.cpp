#include "Demo.hpp"
#include "brasstacks/brasstacks.hpp"

#include "brasstacks/config/RenderConfig.hpp"
#include "brasstacks/rendering/meshes/PlaneMesh.hpp"
#include "brasstacks/rendering/passes/ColorDepthPass.hpp"

#include "brasstacks/rendering/meshes/PlaneMesh.hpp"
#include "brasstacks/rendering/meshes/CubeMesh.hpp"
#include "brasstacks/tools/FPSCamera.hpp"

#include "brasstacks/platform/vulkan/rendering/vkSwapchain.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkPipeline.hpp"
#include "brasstacks/platform/vulkan/resources/vkImage.hpp"
#include "brasstacks/platform/vulkan/resources/vkImageView.hpp"
#include "brasstacks/platform/vulkan/resources/vkSampler.hpp"
#include "brasstacks/platform/vulkan/resources/vkBuffer.hpp"

#include "brasstacks/platform/vulkan/descriptors/vkDescriptorPool.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorSetLayout.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorSet.hpp"

// =============================================================================
Demo::Demo() :
    Application("Demo"),
    _renderer           { nullptr },
    _color_depth_pass   { nullptr },
    _plane_mesh         { nullptr },
    _plane_mat          { },
    _cube_mesh          { nullptr },
    _cube_mat           { },
    _descriptor_pool    { nullptr },
    _texture            { nullptr },
    _texture_view       { nullptr },
    _texture_sampler    { nullptr },
    _texture_set_layout { nullptr },
    _texture_set        { nullptr },
    _camera             { nullptr },
    _camera_ubos        { },
    _camera_ubo_layout  { nullptr },
    _camera_ubo_sets    { nullptr }
{ }

// =============================================================================
void Demo::init(btx::Renderer const &renderer) {
    _renderer = &renderer;

    _descriptor_pool = new btx::vkDescriptorPool(
        _renderer->device(), { }, 1000u,
        {
            { vk::DescriptorType::eUniformBuffer,        1000u, },
            { vk::DescriptorType::eCombinedImageSampler, 1000u, },
        }
    );

    _plane_mesh = new btx::PlaneMesh(
        _renderer->device(),
        0.75f,
        {{
            { 1.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f },
            { 0.25f, 0.25f, 0.25f }
        }}
    );

    _cube_mesh = new btx::CubeMesh(
        _renderer->device(),
        0.75f,
        {{
            { 1.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f },
            { 0.25f, 0.25f, 0.25f },
            { 1.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f },
            { 0.5f, 0.5f, 0.5f },
        }}
    );

    _create_camera();

    _color_depth_pass = new btx::ColorDepthPass(*_renderer);

    _color_depth_pass->pipeline()
        .module_from_spirv("shaders/demo.vert",
                           vk::ShaderStageFlagBits::eVertex)
        .module_from_spirv("shaders/demo.frag",
                           vk::ShaderStageFlagBits::eFragment)
        .describe_vertex_input(btx::Vertex::bindings,
                               btx::Vertex::attributes)
        .add_descriptor_set(*_camera_ubo_layout)
        .add_push_constant(vk::ShaderStageFlagBits::eVertex,
                           sizeof(btx::math::Mat4));

    _color_depth_pass->create();
}

// =============================================================================
void Demo::shutdown() {
    delete _cube_mesh;
    delete _plane_mesh;
    delete _descriptor_pool;
    delete _color_depth_pass;

    _destroy_camera();
}

// =============================================================================
void Demo::update() {
    _camera->update();

    _plane_mat = btx::math::translate(btx::math::Mat4::identity,
                                      -btx::math::Vec3::unit_x);

    // _cube_mat =
    //     math::rotate(math::Mat4::identity,
    //                  20.0f * Timekeeper::run_time(),
    //                  math::Vec3::unit_z) *
    //     math::rotate(math::Mat4::identity,
    //                  10.0f * Timekeeper::run_time(),
    //                  math::Vec3::unit_y);

    _cube_mat = btx::math::translate(btx::math::Mat4::identity,
                                     btx::math::Vec3::unit_x);
}

// =============================================================================
void Demo::record_commands()
{
    auto const image_index = _renderer->image_index();

    std::array<btx::math::Mat4, 2> const vp {{ _camera->view_matrix(),
                                               _camera->proj_matrix() }};

    _camera_ubos[image_index]->fill_buffer(vp.data());

    _color_depth_pass->begin();

        _color_depth_pass->bind_descriptor_set(*_camera_ubo_sets[image_index]);

        _color_depth_pass->send_push_constants({
            btx::ColorDepthPass::PushConstant {
                .stage_flags = vk::ShaderStageFlagBits::eVertex,
                .size_bytes = sizeof(btx::math::Mat4),
                .data = &_plane_mat,
            }
        });

        _plane_mesh->draw_indexed(_renderer->cmd_buffer());

        _color_depth_pass->send_push_constants({
            btx::ColorDepthPass::PushConstant {
                .stage_flags = vk::ShaderStageFlagBits::eVertex,
                .size_bytes = sizeof(btx::math::Mat4),
                .data = &_cube_mat,
            }
        });

        _cube_mesh->draw_indexed(_renderer->cmd_buffer());

    _color_depth_pass->end();
}

// =============================================================================
void Demo::destroy_swapchain_resources() {
    _color_depth_pass->destroy_swapchain_resources();
}

// =============================================================================
void Demo::recreate_swapchain_resources() {
    _color_depth_pass->recreate_swapchain_resources();

    _camera->set_perspective_proj({
        .vfov_degrees = 45.0f,
        .aspect_ratio = _renderer->swapchain().aspect_ratio(),
        .near_plane = 0.1f,
        .far_plane = 1000.0f,
    });
}

// =============================================================================
void Demo::_create_camera() {
    _camera = new btx::FPSCamera(
       {
            .position = { 0.0f, 0.0f, 4.0f },
            .forward  = { 0.0f, 0.0f, 0.0f },
            .up       = { 0.0f, 1.0f, 0.0f },

            .pitch = 0.0f,
            .yaw = -90.0f,
        },
        {
            .vfov_degrees = 45.0f,
            .aspect_ratio = _renderer->swapchain().aspect_ratio(),
            .near_plane = 0.1f,
            .far_plane = 1000.0f,
        }
    );

    auto const image_count = _renderer->swapchain().images().size();
    for(uint32_t i = 0; i < image_count; ++i) {
        _camera_ubos.push_back(new btx::vkBuffer(
            _renderer->device(),
            2 * sizeof(btx::math::Mat4),
            vk::BufferUsageFlagBits::eUniformBuffer,
            (vk::MemoryPropertyFlagBits::eHostVisible |
             vk::MemoryPropertyFlagBits::eHostCoherent)
        ));
    }

    _camera_ubo_layout = new btx::vkDescriptorSetLayout(_renderer->device());

    (*_camera_ubo_layout)
        .add_binding(vk::DescriptorType::eUniformBuffer,
                     vk::ShaderStageFlagBits::eAll)
        .create();

    _camera_ubo_sets.resize(image_count);

    for(uint32_t i = 0; i < image_count; ++i) {
        _camera_ubo_sets[i] =  new btx::vkDescriptorSet(_renderer->device(),
                                                        *_descriptor_pool,
                                                        *_camera_ubo_layout);

        (*_camera_ubo_sets[i])
            .add_buffer(*_camera_ubos[i], vk::DescriptorType::eUniformBuffer)
            .write_set();
    }
}

// =============================================================================
void Demo::_destroy_camera() {
    for(auto *set : _camera_ubo_sets) {
        delete set;
    }
    _camera_ubo_sets.clear();

    delete _camera_ubo_layout;
    _camera_ubo_layout = nullptr;

    for(auto *buffer : _camera_ubos) {
        delete buffer;
    }
    _camera_ubos.clear();

    delete _camera;
    _camera = nullptr;
}