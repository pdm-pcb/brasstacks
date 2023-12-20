#include "brasstacks/core.hpp"
#include "brasstacks/tools/FPSCamera.hpp"

#include "brasstacks/platform/vulkan/devices/vkDevice.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorPool.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorSetLayout.hpp"
#include "brasstacks/platform/vulkan/descriptors/vkDescriptorSet.hpp"
#include "brasstacks/platform/vulkan/resources/buffers/vkBuffer.hpp"
#include "brasstacks/events/EventBroker.hpp"
#include "brasstacks/events/keyboard_events.hpp"
#include "brasstacks/events/mouse_events.hpp"

namespace btx {

// =============================================================================
FPSCamera::FPSCamera(vkDevice const &device,
                     vkDescriptorPool const &descriptor_pool,
                     size_t const image_count,
                     Orientation const &orientation) :
    _device { device }
{
    _allocate_descriptors(descriptor_pool, image_count);
    _register_callbacks();

    _state = {
        .position = orientation.position,
        .forward  = orientation.forward,
        .up       = orientation.up,
    };
}

// =============================================================================
FPSCamera::~FPSCamera() {
    for(auto *set : _camera_ubo_sets) {
        delete set;
    }
    delete _camera_ubo_layout;
    for(auto *buffer : _camera_ubos) {
        delete buffer;
    }
}

// =============================================================================
void FPSCamera::update() {
    auto const cos_yaw   = std::cos(math::radians(_state.yaw));
    auto const sin_yaw   = std::sin(math::radians(_state.yaw));
    auto const cos_pitch = std::cos(math::radians(_state.pitch));
    auto const sin_pitch = std::sin(math::radians(_state.pitch));

    _state.forward.x = cos_yaw * cos_pitch;
    _state.forward.y = sin_pitch;
    _state.forward.z = sin_yaw * cos_pitch;

    _state.forward = math::normalize(_state.forward);
    _state.side = math::normalize(
        math::cross(_state.forward, math::Vec3::unit_y)
    );
    _state.up = math::cross(_state.side, _state.forward);

    auto move_speed = _config.move_speed * Timekeeper::frame_time();
    if(_kb.lshift) {
        move_speed *= 2.0f;
    }

    if(_kb.w)     { _state.position += _state.forward * move_speed; }
    if(_kb.s)     { _state.position -= _state.forward * move_speed; }
    if(_kb.a)     { _state.position -= _state.side    * move_speed; }
    if(_kb.d)     { _state.position += _state.side    * move_speed; }
    if(_kb.space) { _state.position += _state.up      * move_speed; }
    if(_kb.lctrl) { _state.position -= _state.up      * move_speed; }
}

// =============================================================================
void FPSCamera::send_buffer_data(uint32_t const image_index) {
    // static auto const start = std::chrono::high_resolution_clock::now();
    // auto const now = std::chrono::high_resolution_clock::now();
    // auto const duration = std::chrono::duration<float, std::chrono::seconds::period>(now - start).count();
    // auto const model_matrix = math::rotate(math::Mat4::identity, 20.0f * duration, math::Vec3::unit_z) *
    // math::rotate(math::Mat4::identity, 10.0f * duration, math::Vec3::unit_y);

    auto const model_matrix = math::Mat4::identity;

    _view_matrix = math::orient_view_rh(
        _state.position,
        _state.forward,
        _state.side,
        _state.up
    );

    _proj_matrix = math::persp_proj_rh_no(_config.vfov_degrees,
                                          _config.aspect_ratio,
                                          _config.near_plane,
                                          _config.far_plane);

    std::array<math::Mat4, 3> const mvp {{
        model_matrix, _view_matrix, _proj_matrix
    }};

    _camera_ubos[image_index]->fill_buffer(mvp.data());
}

// =============================================================================
void FPSCamera::on_key_press(KeyPressEvent const &event) {
    switch(event.code) {
        case BTX_KB_W          : _kb.w      = true; break;
        case BTX_KB_A          : _kb.a      = true; break;
        case BTX_KB_S          : _kb.s      = true; break;
        case BTX_KB_D          : _kb.d      = true; break;
        case BTX_KB_LEFT_CTRL  : _kb.lctrl  = true; break;
        case BTX_KB_SPACE      : _kb.space  = true; break;
        case BTX_KB_LEFT_SHIFT : _kb.lshift = true; break;
        default: break;
    }
}

// =============================================================================
void FPSCamera::on_key_release(KeyReleaseEvent const &event) {
    switch(event.code) {
        case BTX_KB_W          : _kb.w      = false; break;
        case BTX_KB_A          : _kb.a      = false; break;
        case BTX_KB_S          : _kb.s      = false; break;
        case BTX_KB_D          : _kb.d      = false; break;
        case BTX_KB_LEFT_CTRL  : _kb.lctrl  = false; break;
        case BTX_KB_SPACE      : _kb.space  = false; break;
        case BTX_KB_LEFT_SHIFT : _kb.lshift = false; break;
        default: break;
    }
}

// =============================================================================
void FPSCamera::on_mouse_move(MouseMoveEvent const &event) {
    _state.pitch += static_cast<float>(-event.y_offset) * _config.look_speed;
    _state.yaw   += static_cast<float>(event.x_offset) * _config.look_speed;

    if(_state.pitch > 89.0f)       { _state.pitch = 89.0f;  }
    else if(_state.pitch < -89.0f) { _state.pitch = -89.0f; }

    if(_state.yaw > 360.0f)    { _state.yaw -= 360.0f; }
    else if(_state.yaw < 0.0f) { _state.yaw += 360.0f; }
}

// =============================================================================
void FPSCamera::_allocate_descriptors(vkDescriptorPool const &descriptor_pool,
                                      size_t const image_count)
{
    for(uint32_t i = 0; i < image_count; ++i) {
        _camera_ubos.push_back(new vkBuffer(
            _device, sizeof(float) * 16 * 3,
            vk::BufferUsageFlagBits::eUniformBuffer,
            (vk::MemoryPropertyFlagBits::eHostVisible |
             vk::MemoryPropertyFlagBits::eHostCoherent)
        ));
    }

    _camera_ubo_layout = new vkDescriptorSetLayout(_device);

    (*_camera_ubo_layout)
        .add_binding(vk::DescriptorType::eUniformBuffer,
                     vk::ShaderStageFlagBits::eAll)
        .create();

    _camera_ubo_sets.resize(image_count);

    for(uint32_t i = 0; i < image_count; ++i) {
        _camera_ubo_sets[i] =
            new vkDescriptorSet(_device, descriptor_pool, *_camera_ubo_layout);

        (*_camera_ubo_sets[i])
            .allocate()
            .add_buffer(*_camera_ubos[i], vk::DescriptorType::eUniformBuffer)
            .write_set();
    }
}

// =============================================================================
void FPSCamera::_register_callbacks() {
    EventBroker::subscribe<KeyPressEvent>(this, &FPSCamera::on_key_press);
    EventBroker::subscribe<KeyReleaseEvent>(this, &FPSCamera::on_key_release);
    EventBroker::subscribe<MouseMoveEvent>(this, &FPSCamera::on_mouse_move);
}

} // namespace btx