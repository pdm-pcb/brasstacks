#include "brasstacks/brasstacks.hpp"
#include "brasstacks/tools/cameras/CameraController.hpp"

#include "brasstacks/core/Renderer.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorPool.hpp"

namespace btx {
PerspectiveCamera *CameraController::_perspective_camera { nullptr };

std::vector<std::unique_ptr<vmaBuffer>> CameraController::_camera_ubos { };

auto CameraController::_camera_ubo_layout {
    std::make_unique<vkDescriptorSetLayout>()
};

std::vector<std::unique_ptr<vkDescriptorSet>>
    CameraController::_camera_ubo_sets { };

// =============================================================================
void CameraController::init() {
    _perspective_camera = new PerspectiveCamera;
    _perspective_camera->orient(
        PerspectiveCamera::Orientation {
            .position = { 0.0f, 0.0f, 4.0f },
            .forward  = { 0.0f, 0.0f, 0.0f },
            .up       = { 0.0f, 1.0f, 0.0f },

            .pitch = 0.0f,
            .yaw = -90.0f,
        }
    );
    _perspective_camera->set_perspective(
        PerspectiveCamera::PerspectiveParams {
            .vfov_degrees = 45.0f,
            .aspect_ratio = Renderer::render_surface_aspect_ratio(),
            .near_plane = 0.1f,
            .far_plane = 1000.0f,
        }
    );

    _perspective_camera->update();

    auto const image_count = Renderer::swapchain().images().size();

    if(_camera_ubos.size() != image_count) {
        _camera_ubos.reserve(image_count);

        // Fill in the buffers with in-place default construction
        std::generate_n(
            std::back_inserter(_camera_ubos),
            _camera_ubos.capacity(),
            []() {
                return std::make_unique<vmaBuffer>();
            }
        );
    }

    for(auto &buffer : _camera_ubos) {
        buffer->create(2 * sizeof(math::Mat4),
                       vk::BufferUsageFlagBits::eUniformBuffer,
                       VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
                       (vk::MemoryPropertyFlagBits::eHostVisible |
                        vk::MemoryPropertyFlagBits::eHostCoherent));
    }

    (*_camera_ubo_layout)
        .add_binding(vk::DescriptorType::eUniformBuffer,
                     vk::ShaderStageFlagBits::eAll)
        .create();

    if(_camera_ubo_sets.size() != image_count) {
        _camera_ubo_sets.reserve(image_count);

        // Fill in the buffers with in-place default construction
        std::generate_n(
            std::back_inserter(_camera_ubo_sets),
            _camera_ubo_sets.capacity(),
            []() {
                return std::make_unique<vkDescriptorSet>();
            }
        );
    }

    for(uint32_t i = 0; i < image_count; ++i) {
        _camera_ubo_sets[i]->allocate(Renderer::descriptor_pool(),
                                      *_camera_ubo_layout);

        (*_camera_ubo_sets[i])
            .add_buffer(*_camera_ubos[i], vk::DescriptorType::eUniformBuffer)
            .write_set();
    }
}

// =============================================================================
void CameraController::shutdown() {
    _camera_ubo_sets.clear();
    _camera_ubo_layout->destroy();

    for(auto &buffer : _camera_ubos) {
        buffer->destroy();
    }

    delete _perspective_camera;
    _perspective_camera = nullptr;
}

// =============================================================================
void CameraController::update_perspective() {
    _perspective_camera->set_perspective({
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