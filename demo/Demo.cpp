#include "Demo.hpp"
#include "brasstacks/core.hpp"

#include "brasstacks/platform/vulkan/descriptors/vkDescriptorPool.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorSetLayout.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorSet.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"
#include "brasstacks/platform/vulkan/rendering/vkSwapchain.hpp"
#include "brasstacks/platform/vulkan/rendering/vkRenderPass.hpp"
#include "brasstacks/platform/vulkan/rendering/vkFramebuffer.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkPipeline.hpp"
#include "brasstacks/platform/vulkan/resources/buffers/vkBuffer.hpp"
#include "brasstacks/rendering/RenderPass.hpp"

#include "brasstacks/rendering/meshes/CubeMesh.hpp"
#include "brasstacks/tools/FPSCamera.hpp"

// =============================================================================
Demo::Demo() :
    btx::Application("Demo"),
    _descriptor_pool   { nullptr },
    _render_pass       { nullptr },
    _pipeline          { nullptr },
    _framebuffers      { },
    _cube_mesh         { nullptr },
    _cube_mat          { btx::math::Mat4::identity },
    _camera            { nullptr },
    _camera_ubo_layout { nullptr },
    _camera_ubo_sets   { },
    _camera_ubos       { }
{ }

// =============================================================================
Demo::~Demo() {
    _destroy_render_pass();
    _destroy_camera_resources();

    delete _cube_mesh;
    delete _camera;

    delete _descriptor_pool;
}

// =============================================================================
void Demo::init(btx::vkDevice const &device, btx::vkSwapchain const &swapchain)
{
    BTX_TRACE("client app init");

    _descriptor_pool = new btx::vkDescriptorPool(
        device,
        1000u,
        {
            { vk::DescriptorType::eUniformBuffer,        1000u, },
            { vk::DescriptorType::eCombinedImageSampler, 1000u, },
        }
    );

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
            .aspect_ratio = btx::RenderConfig::swapchain_aspect_ratio,
            .near_plane = 0.1f,
            .far_plane = 1000.0f,
        }
    );

    _create_camera_resources(device);

    _cube_mesh = new btx::CubeMesh(
        device,
        0.5f,
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

    _create_render_pass(device, swapchain);
}

// =============================================================================
void Demo::shutdown() {
    BTX_TRACE("client app shutdown");
}

// =============================================================================
void Demo::update() {
    _camera->update();

    // _cube_mat =
    //     math::rotate(math::Mat4::identity,
    //                  20.0f * Timekeeper::run_time(),
    //                  math::Vec3::unit_z) *
    //     math::rotate(math::Mat4::identity,
    //                  10.0f * Timekeeper::run_time(),
    //                  math::Vec3::unit_y);

    _cube_mat = btx::math::Mat4::identity;
}

void Demo::record_commands(btx::vkCmdBuffer const &cmd_buffer,
                           uint32_t const image_index)
{
    std::array<btx::math::Mat4, 2> const vp {{
        _camera->view_matrix(), _camera->proj_matrix()
    }};

    _camera_ubos[image_index]->fill_buffer(vp.data());

    auto const &framebuffer = *_framebuffers[image_index];

    static vk::ClearValue const clear_value {
        .color = { btx::RenderConfig::clear_color }
    };

    vk::Rect2D const render_area = {
        .offset {
            .x = btx::RenderConfig::swapchain_image_offset.x,
            .y = btx::RenderConfig::swapchain_image_offset.y,
        },
        .extent {
            .width  = btx::RenderConfig::swapchain_image_size.width,
            .height = btx::RenderConfig::swapchain_image_size.height,
        }
    };

    cmd_buffer.begin_render_pass(
        vk::RenderPassBeginInfo {
            .pNext           = nullptr,
            .renderPass      = _render_pass->native(),
            .framebuffer     = framebuffer.native(),
            .renderArea      = render_area,
            .clearValueCount = 1u,
            .pClearValues    = &clear_value,
        }
    );

    _pipeline->bind(cmd_buffer);

    _pipeline->bind_descriptor_set(cmd_buffer, *_camera_ubo_sets[image_index]);

    _send_push_constants(cmd_buffer, {
        PushConstant {
            .stage_flags = vk::ShaderStageFlagBits::eVertex,
            .size_bytes = sizeof(btx::math::Mat4),
            .data = &_cube_mat,
        }
    });

    _cube_mesh->draw_indexed(cmd_buffer);

    cmd_buffer.end_render_pass();
}

// =============================================================================
void Demo::_send_push_constants(btx::vkCmdBuffer const &cmd_buffer,
                                PushConstants const &push_constants)
{
    size_t offset = 0u;
    for(auto const& push_constant : push_constants) {
        cmd_buffer.native().pushConstants(
            _pipeline->layout(),
            push_constant.stage_flags,
            static_cast<uint32_t>(offset),
            static_cast<uint32_t>(push_constant.size_bytes),
            push_constant.data
        );

        offset += push_constant.size_bytes;
    }
}

// =============================================================================
void Demo::_create_camera_resources(btx::vkDevice const &device) {
    for(uint32_t i = 0; i < btx::RenderConfig::swapchain_image_count; ++i) {
        _camera_ubos.push_back(new btx::vkBuffer(
            device, 2 * sizeof(btx::math::Mat4),
            vk::BufferUsageFlagBits::eUniformBuffer,
            (vk::MemoryPropertyFlagBits::eHostVisible |
             vk::MemoryPropertyFlagBits::eHostCoherent)
        ));
    }

    _camera_ubo_layout = new btx::vkDescriptorSetLayout(device);

    (*_camera_ubo_layout)
        .add_binding(vk::DescriptorType::eUniformBuffer,
                     vk::ShaderStageFlagBits::eAll)
        .create();

    _camera_ubo_sets.resize(btx::RenderConfig::swapchain_image_count);

    for(uint32_t i = 0; i < btx::RenderConfig::swapchain_image_count; ++i) {
        _camera_ubo_sets[i] =  new btx::vkDescriptorSet(device,
                                                        *_descriptor_pool,
                                                        *_camera_ubo_layout);

        (*_camera_ubo_sets[i])
            .allocate()
            .add_buffer(*_camera_ubos[i], vk::DescriptorType::eUniformBuffer)
            .write_set();
    }
}

// =============================================================================
void Demo::_destroy_camera_resources() {
    for(auto *set : _camera_ubo_sets) {
        delete set;
    }
    delete _camera_ubo_layout;
    for(auto *buffer : _camera_ubos) {
        delete buffer;
    }
}

// =============================================================================
void Demo::_create_render_pass(btx::vkDevice const &device,
                               btx::vkSwapchain const &swapchain)
{
    _render_pass = new btx::vkRenderPass(device, swapchain.image_format());

    _pipeline = new btx::vkPipeline(device);
    (*_pipeline)
        .module_from_spirv("shaders/demo.vert",
                           vk::ShaderStageFlagBits::eVertex)
        .module_from_spirv("shaders/demo.frag",
                           vk::ShaderStageFlagBits::eFragment)
        .describe_vertex_input(btx::Vertex::bindings, btx::Vertex::attributes)
        .add_descriptor_set(*_camera_ubo_layout)
        .add_push_constant(
            vk::ShaderStageFlagBits::eVertex,
            sizeof(btx::math::Mat4)
        )
        .create(
            *_render_pass,
            {
                .color_formats = { swapchain.image_format() },
                .depth_format    = vk::Format::eUndefined,
                .viewport_extent = {
                    .width  = btx::RenderConfig::swapchain_image_size.width,
                    .height = btx::RenderConfig::swapchain_image_size.height,
                },
                .viewport_offset = {
                    .x = btx::RenderConfig::swapchain_image_offset.x,
                    .y = btx::RenderConfig::swapchain_image_offset.y,
                },
                .sample_flags    = vk::SampleCountFlagBits::e1,
            }
        );

    for(auto const *image : swapchain.images()) {
        _framebuffers.push_back(new btx::vkFramebuffer(
            device,
            *_render_pass,
            {
                .width  = btx::RenderConfig::swapchain_image_size.width,
                .height = btx::RenderConfig::swapchain_image_size.height,
            },
            *image
        ));
    }
}

// =============================================================================
void Demo::_destroy_render_pass() {
    for(auto *framebuffer : _framebuffers) {
        delete framebuffer;
    }

    delete _pipeline;
    delete _render_pass;
}