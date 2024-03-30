#include "brasstacks/brasstacks.hpp"
#include "brasstacks/tools/cameras/CameraController.hpp"

#include "brasstacks/core/Renderer.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorPool.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorSetLayout.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorSet.hpp"

namespace btx {
PerspectiveCamera *CameraController::_perspective_camera { nullptr };

std::vector<vkBuffer *>        CameraController::_camera_ubos       { };
vkDescriptorSetLayout         *CameraController::_camera_ubo_layout { nullptr };
std::vector<vkDescriptorSet *> CameraController::_camera_ubo_sets   { };

// =============================================================================
void CameraController::init() {
    _perspective_camera = new PerspectiveCamera(
       PerspectiveCamera::Orientation {
            .position = { 0.0f, 0.0f, 4.0f },
            .forward  = { 0.0f, 0.0f, 0.0f },
            .up       = { 0.0f, 1.0f, 0.0f },

            .pitch = 0.0f,
            .yaw = -90.0f,
        },
        PerspectiveCamera::PerspectiveParams {
            .vfov_degrees = 45.0f,
            .aspect_ratio = Renderer::render_surface_aspect_ratio(),
            .near_plane = 0.1f,
            .far_plane = 1000.0f,
        }
    );

    _perspective_camera->update();

    auto const image_count = Renderer::swapchain().images().size();
    for(uint32_t i = 0; i < image_count; ++i) {
        _camera_ubos.push_back(new vkBuffer(
            2 * sizeof(math::Mat4),
            vk::BufferUsageFlagBits::eUniformBuffer,
            (vk::MemoryPropertyFlagBits::eHostVisible |
             vk::MemoryPropertyFlagBits::eHostCoherent)
        ));
    }

    _camera_ubo_layout = new vkDescriptorSetLayout;

    (*_camera_ubo_layout)
        .add_binding(vk::DescriptorType::eUniformBuffer,
                     vk::ShaderStageFlagBits::eAll)
        .create();

    _camera_ubo_sets.resize(image_count);

    for(uint32_t i = 0; i < image_count; ++i) {
        _camera_ubo_sets[i] =  new vkDescriptorSet(Renderer::descriptor_pool(),
                                                   *_camera_ubo_layout);

        (*_camera_ubo_sets[i])
            .add_buffer(*_camera_ubos[i], vk::DescriptorType::eUniformBuffer)
            .write_set();
    }
}

// =============================================================================
void CameraController::shutdown() {
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

    delete _perspective_camera;
    _perspective_camera = nullptr;
}

// =============================================================================
void CameraController::update_perspective() {
    _perspective_camera->set_perspective_proj({
        .vfov_degrees = 45.0f,
        .aspect_ratio = Renderer::swapchain().aspect_ratio(),
        .near_plane = 0.1f,
        .far_plane = 1000.0f,
    });
}

// =============================================================================
void CameraController::update_ubo() {
    static std::array<btx::math::Mat4, 2> vp;

    vp = {{
        _perspective_camera->view_matrix(),
        _perspective_camera->proj_matrix()
    }};

    _camera_ubos[Renderer::image_index()]->fill_buffer(vp.data());
}

} // namespace btx