#include "brasstacks/brasstacks.hpp"
#include "brasstacks/tools/cameras/CameraController.hpp"

#include "brasstacks/core/Renderer.hpp"
#include "brasstacks/tools/cameras/PerspectiveCamera.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorPool.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorSetLayout.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorSet.hpp"

namespace btx {

// =============================================================================
CameraController::CameraController(Renderer const &renderer) :
    _renderer { renderer}
{
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
            .aspect_ratio = _renderer.render_surface_aspect_ratio(),
            .near_plane = 0.1f,
            .far_plane = 1000.0f,
        }
    );

    auto const image_count = renderer.swapchain().images().size();
    for(uint32_t i = 0; i < image_count; ++i) {
        _camera_ubos.push_back(new vkBuffer(
             renderer.device(),
            2 * sizeof(math::Mat4),
            vk::BufferUsageFlagBits::eUniformBuffer,
            (vk::MemoryPropertyFlagBits::eHostVisible |
             vk::MemoryPropertyFlagBits::eHostCoherent)
        ));
    }

    _camera_ubo_layout = new vkDescriptorSetLayout(renderer.device());

    (*_camera_ubo_layout)
        .add_binding(vk::DescriptorType::eUniformBuffer,
                     vk::ShaderStageFlagBits::eAll)
        .create();

    _camera_ubo_sets.resize(image_count);

    for(uint32_t i = 0; i < image_count; ++i) {
        _camera_ubo_sets[i] =  new vkDescriptorSet(renderer.device(),
                                                   *_descriptor_pool,
                                                   *_camera_ubo_layout);

        (*_camera_ubo_sets[i])
            .add_buffer(*_camera_ubos[i], vk::DescriptorType::eUniformBuffer)
            .write_set();
    }
}

// =============================================================================
CameraController::~CameraController() {
    delete _perspective_camera;
}

} // namespace btx