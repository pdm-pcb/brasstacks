#include "Demo.hpp"
#include "brasstacks/platform/vulkan/passes/vkColorDepthPass.hpp"
#include "brasstacks/platform/vulkan/passes/vkColorDepthResolvePass.hpp"

// =============================================================================
Demo::Demo() :
    Application("Brasstacks"),
    _pipeline   { },
    _plane_mesh { },
    _plane_mat  { },
    _cube_mesh  { },
    _cube_mat   { },
    _texture    { }
{ }

// =============================================================================
void Demo::init() {
    _plane_mesh = btx::MeshLibrary::new_plane_mesh(0.75f);
    _cube_mesh = btx::MeshLibrary::new_cube_mesh(0.75f);

    _texture =
        btx::TextureLibrary::load_texture("textures/woodfloor_051_d.jpg");

    create_pipeline();
}

// =============================================================================
void Demo::shutdown() {
    destroy_pipeline();
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
void Demo::record_commands() {
    static std::array<btx::vkPipeline::PushConstant const, 1> const plane_pcs {
        btx::vkPipeline::PushConstant {
            .stage_flags = vk::ShaderStageFlagBits::eVertex,
            .size_bytes = sizeof(btx::math::Mat4),
            .data = &_plane_mat,
        }
    };

    static std::array<btx::vkPipeline::PushConstant const, 1> const cube_pcs {
        btx::vkPipeline::PushConstant {
            .stage_flags = vk::ShaderStageFlagBits::eVertex,
            .size_bytes = sizeof(btx::math::Mat4),
            .data = &_cube_mat,
        }
    };

    _pipeline.bind(btx::Renderer::cmd_buffer());

        _pipeline.bind_descriptor_set(btx::CameraController::camera_ubo_set());
        _pipeline.bind_descriptor_set((*_texture)->descriptor_set());

        _pipeline.send_push_constants(plane_pcs);
        (*_plane_mesh)->draw_indexed(btx::Renderer::cmd_buffer());

        _pipeline.send_push_constants(cube_pcs);
        (*_cube_mesh)->draw_indexed(btx::Renderer::cmd_buffer());

    _pipeline.unbind();
}

// =============================================================================
void Demo::swapchain_updated() {
    _pipeline.update_dimensions(btx::Renderer::swapchain().size(),
                                 btx::Renderer::swapchain().offset());
}

// =============================================================================
void Demo::create_pipeline() {
    if(btx::RenderConfig::current_msaa->msaa > 1u) {
        auto const &render_pass =
            dynamic_cast<btx::vkColorDepthResolvePass const &>(
                btx::Renderer::render_pass()
            );

    _pipeline
        .module_from_spirv("shaders/demo.vert",
                           vk::ShaderStageFlagBits::eVertex)
        .module_from_spirv("shaders/demo.frag",
                           vk::ShaderStageFlagBits::eFragment)
        .describe_vertex_input(btx::Vertex::bindings,
                               btx::Vertex::attributes)
        .add_descriptor_set_layout(btx::CameraController::camera_ubo_layout())
        .add_descriptor_set_layout((*_texture)->descriptor_set_layout())
        .add_push_constant({ .stage_flags = vk::ShaderStageFlagBits::eVertex,
                             .size_bytes = sizeof(btx::math::Mat4) })
        .create(
            render_pass,
            {
                .color_formats     = { render_pass.color_format() },
                .depth_format      = render_pass.depth_format(),
                .viewport_extent   = btx::Renderer::swapchain().size(),
                .viewport_offset   = btx::Renderer::swapchain().offset(),
                .sample_flags      = render_pass.msaa_samples(),
                .enable_depth_test = VK_TRUE,
            }
        );
    }
    else {
        auto const &render_pass =
            dynamic_cast<btx::vkColorDepthPass const &>(
                btx::Renderer::render_pass()
            );

    _pipeline
        .module_from_spirv("shaders/demo.vert",
                           vk::ShaderStageFlagBits::eVertex)
        .module_from_spirv("shaders/demo.frag",
                           vk::ShaderStageFlagBits::eFragment)
        .describe_vertex_input(btx::Vertex::bindings,
                               btx::Vertex::attributes)
        .add_descriptor_set_layout(btx::CameraController::camera_ubo_layout())
        .add_descriptor_set_layout((*_texture)->descriptor_set_layout())
        .add_push_constant({ .stage_flags = vk::ShaderStageFlagBits::eVertex,
                             .size_bytes = sizeof(btx::math::Mat4) })
        .create(
            render_pass,
            {
                .color_formats     = { render_pass.color_format() },
                .depth_format      = render_pass.depth_format(),
                .viewport_extent   = btx::Renderer::swapchain().size(),
                .viewport_offset   = btx::Renderer::swapchain().offset(),
                .sample_flags      = vk::SampleCountFlagBits::e1,
                .enable_depth_test = VK_TRUE,
            }
        );
    }
}

// =============================================================================
void Demo::destroy_pipeline() {
    _pipeline.destroy();
}