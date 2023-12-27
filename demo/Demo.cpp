#include "Demo.hpp"
#include "brasstacks/core.hpp"

#include "brasstacks/platform/vulkan/descriptors/vkDescriptorPool.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorSetLayout.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorSet.hpp"
#include "brasstacks/platform/vulkan/devices/vkCmdBuffer.hpp"
#include "brasstacks/platform/vulkan/rendering/vkSwapchain.hpp"
#include "brasstacks/platform/vulkan/rendering/vkColorPass.hpp"
#include "brasstacks/platform/vulkan/rendering/vkFramebuffer.hpp"
#include "brasstacks/platform/vulkan/pipeline/vkPipeline.hpp"
#include "brasstacks/platform/vulkan/resources/vkBuffer.hpp"
#include "brasstacks/platform/vulkan/resources/vkImage.hpp"

#include "brasstacks/rendering/RenderPass.hpp"

#include "brasstacks/rendering/meshes/PlaneMesh.hpp"
#include "brasstacks/rendering/meshes/CubeMesh.hpp"
#include "brasstacks/tools/FPSCamera.hpp"

// =============================================================================
Demo::Demo() :
    btx::Application("Demo"),
    _descriptor_pool    { nullptr },
    _camera             { nullptr },
    _camera_ubos        { },
    _camera_ubo_layout  { nullptr },
    _camera_ubo_sets    { },
    _plane_mesh         { nullptr },
    _plane_mat          { btx::math::Mat4::identity },
    _cube_mesh          { nullptr },
    _cube_mat           { btx::math::Mat4::identity },
    _texture            { nullptr },
    _texture_view       { nullptr },
    _texture_sampler    { nullptr },
    _texture_set_layout { nullptr },
    _texture_set        { nullptr },
    _color_pass   { nullptr },
    _color_pipeline           { nullptr },
    _framebuffers       { }

{ }

// =============================================================================
void Demo::init(btx::vkPhysicalDevice const &physical_device,
                btx::vkDevice const &device, btx::vkSwapchain const &swapchain)
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

    _create_camera(device);

    _plane_mesh = new btx::PlaneMesh(
        device,
        0.75f
        // {{
        //     { 1.0f, 0.0f, 0.0f },
        //     { 0.0f, 1.0f, 0.0f },
        //     { 0.0f, 0.0f, 1.0f },
        //     { 0.25f, 0.25f, 0.25f }
        // }}
    );

    _cube_mesh = new btx::CubeMesh(
        device,
        0.75f
        // {{
        //     { 1.0f, 0.0f, 0.0f },
        //     { 0.0f, 1.0f, 0.0f },
        //     { 0.0f, 0.0f, 1.0f },
        //     { 0.25f, 0.25f, 0.25f },
        //     { 1.0f, 0.0f, 0.0f },
        //     { 0.0f, 1.0f, 0.0f },
        //     { 0.0f, 0.0f, 1.0f },
        //     { 0.5f, 0.5f, 0.5f },
        // }}
    );

    _create_texture(device);

    _create_render_pass(physical_device, device, swapchain);
}

// =============================================================================
void Demo::shutdown() {
    BTX_TRACE("client app shutdown");

    _destroy_render_pass();
    _destroy_camera();
    _destroy_texture();

    delete _cube_mesh;
    delete _plane_mesh;

    delete _descriptor_pool;
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
            .renderPass      = _color_pass->native(),
            .framebuffer     = framebuffer.native(),
            .renderArea      = render_area,
            .clearValueCount = 1u,
            .pClearValues    = &clear_value,
        }
    );

    _color_pipeline->bind(cmd_buffer);
    _color_pipeline->bind_descriptor_set(cmd_buffer, *_camera_ubo_sets[image_index]);
    _color_pipeline->bind_descriptor_set(cmd_buffer, *_texture_set);

    _send_push_constants(cmd_buffer, {
        PushConstant {
            .stage_flags = vk::ShaderStageFlagBits::eVertex,
            .size_bytes = sizeof(btx::math::Mat4),
            .data = &_plane_mat,
        }
    });
    _plane_mesh->draw_indexed(cmd_buffer);

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
            _color_pipeline->layout(),
            push_constant.stage_flags,
            static_cast<uint32_t>(offset),
            static_cast<uint32_t>(push_constant.size_bytes),
            push_constant.data
        );

        offset += push_constant.size_bytes;
    }
}

// =============================================================================
void Demo::_create_camera(btx::vkDevice const &device) {
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
            .add_buffer(*_camera_ubos[i], vk::DescriptorType::eUniformBuffer)
            .write_set();
    }
}

// =============================================================================
void Demo::_destroy_camera() {
    for(auto *set : _camera_ubo_sets) {
        delete set;
    }

    delete _camera_ubo_layout;

    for(auto *buffer : _camera_ubos) {
        delete buffer;
    }

    delete _camera;
}

// =============================================================================
void Demo::_create_texture(btx::vkDevice const &device) {
    _texture = new btx::vkImage(device, "textures/woodfloor_051_d.jpg");

    _texture->create(vk::ImageType::e2D,
                     vk::SampleCountFlagBits::e1,
                     (vk::ImageUsageFlagBits::eSampled |
                      vk::ImageUsageFlagBits::eTransferDst |
                      vk::ImageUsageFlagBits::eTransferSrc),
                     vk::MemoryPropertyFlagBits::eDeviceLocal);

    _texture_view = new btx::vkImageView(device, *_texture,
                                         vk::ImageViewType::e2D,
                                         vk::ImageAspectFlagBits::eColor);

    _texture_sampler = new btx::vkSampler(device, vk::Filter::eLinear,
                                          vk::Filter::eLinear,
                                          vk::SamplerMipmapMode::eLinear,
                                          vk::SamplerAddressMode::eRepeat,
                                          vk::SamplerAddressMode::eRepeat);

    _texture_set_layout = new btx::vkDescriptorSetLayout(device);

    (*_texture_set_layout)
        .add_binding(vk::DescriptorType::eCombinedImageSampler,
                     vk::ShaderStageFlagBits::eFragment)
        .create();

    // _texture_sets.resize(btx::RenderConfig::swapchain_image_count);

    // for(uint32_t i = 0; i < btx::RenderConfig::swapchain_image_count; ++i) {
        _texture_set =  new btx::vkDescriptorSet(device,
                                                 *_descriptor_pool,
                                                 *_texture_set_layout);

        (*_texture_set)
            .add_image(*_texture, *_texture_view, *_texture_sampler,
                       vk::DescriptorType::eCombinedImageSampler)
            .write_set();
    // }
}

// =============================================================================
void Demo::_destroy_texture() {
    // for(auto *set : _texture_sets) {
        delete _texture_set;
    // }

    delete _texture_set_layout;

    delete _texture_sampler;
    delete _texture_view;
    delete _texture;
}

// =============================================================================
void Demo::_create_render_pass(btx::vkPhysicalDevice const &physical_device,
                               btx::vkDevice const &device,
                               btx::vkSwapchain const &swapchain)
{
    auto const msaa_samples =
        btx::vkPipeline::samples_to_flag(btx::RenderConfig::msaa_samples);

    _color_pass = new btx::vkColorPass(device, swapchain.image_format(),
                                             msaa_samples);

    _color_pipeline = new btx::vkPipeline(device);
    (*_color_pipeline)
        .module_from_spirv("shaders/demo.vert",
                           vk::ShaderStageFlagBits::eVertex)
        .module_from_spirv("shaders/demo.frag",
                           vk::ShaderStageFlagBits::eFragment)
        .describe_vertex_input(btx::Vertex::bindings, btx::Vertex::attributes)
        .add_descriptor_set(*_camera_ubo_layout)
        .add_descriptor_set(*_texture_set_layout)
        .add_push_constant(
            vk::ShaderStageFlagBits::eVertex,
            sizeof(btx::math::Mat4)
        )
        .create(
            *_color_pass,
            {
                .color_formats = { swapchain.image_format() },
                .depth_format = vk::Format::eUndefined,
                .viewport_extent = {
                    .width  = btx::RenderConfig::swapchain_image_size.width,
                    .height = btx::RenderConfig::swapchain_image_size.height,
                },
                .viewport_offset = {
                    .x = btx::RenderConfig::swapchain_image_offset.x,
                    .y = btx::RenderConfig::swapchain_image_offset.y,
                },
                .sample_flags = msaa_samples,
            }
        );

    for(auto const *view : swapchain.image_views()) {
        _framebuffers.push_back(new btx::vkFramebuffer(
            device,
            *_color_pass,
            {
                .width  = btx::RenderConfig::swapchain_image_size.width,
                .height = btx::RenderConfig::swapchain_image_size.height,
            },
            *view
        ));
    }
}

// =============================================================================
void Demo::_destroy_render_pass() {
    for(auto *framebuffer : _framebuffers) {
        delete framebuffer;
    }

    delete _color_pipeline;
    delete _color_pass;
}