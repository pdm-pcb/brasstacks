#include "brasstacks/brasstacks.hpp"
#include "brasstacks/tools/FPSCamera.hpp"

#include "brasstacks/events/EventBus.hpp"
#include "brasstacks/events/keyboard_events.hpp"
#include "brasstacks/events/mouse_events.hpp"

namespace btx {

// =============================================================================
FPSCamera::FPSCamera(Orientation const &orientation,
                     PerspectiveParams const &persp_params) :
    _key_press_queue   { *this, &FPSCamera::on_key_press },
    _key_release_queue { *this, &FPSCamera::on_key_release },
    _mouse_move_queue  { *this, &FPSCamera::on_mouse_move }
{
    _state = {
        .position = orientation.position,
        .forward  = orientation.forward,
        .up       = orientation.up,
    };

    set_perspective_proj(persp_params);
}

// =============================================================================
void FPSCamera::update() {
    _process_events();

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

    auto move_speed = _config.move_speed * TimeKeeper::sim_tick_delta();
    if(_kb.lshift) {
        move_speed *= 3.0f;
    }

    if(_kb.w)     { _state.position += _state.forward * move_speed; }
    if(_kb.s)     { _state.position -= _state.forward * move_speed; }
    if(_kb.a)     { _state.position -= _state.side    * move_speed; }
    if(_kb.d)     { _state.position += _state.side    * move_speed; }
    if(_kb.space) { _state.position += _state.up      * move_speed; }
    if(_kb.lctrl) { _state.position -= _state.up      * move_speed; }

    _view_matrix = math::orient_view_rh(
        _state.position,
        _state.forward,
        _state.side,
        _state.up
    );
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
void FPSCamera::set_perspective_proj(PerspectiveParams const &persp_params) {
    _proj_matrix = math::persp_proj_rh_zo(
        persp_params.vfov_degrees,
        persp_params.aspect_ratio,
        persp_params.near_plane,
        persp_params.far_plane
    );
}

// =============================================================================
void FPSCamera::_process_events() {
    _key_press_queue.process_queue();
    _key_release_queue.process_queue();
    _mouse_move_queue.process_queue();
}

} // namespace btx