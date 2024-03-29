#include "brasstacks/brasstacks.hpp"
#include "brasstacks/tools/cameras/PerspectiveCamera.hpp"

#include "brasstacks/events/EventBus.hpp"
#include "brasstacks/events/keyboard_events.hpp"
#include "brasstacks/events/mouse_events.hpp"

namespace btx {

// =============================================================================
PerspectiveCamera::PerspectiveCamera(Orientation const &orientation,
                                     PerspectiveParams const &persp_params) :
    _keyboard_events   { *this, &PerspectiveCamera::keyboard_event },
    _mouse_move_events { *this, &PerspectiveCamera::mouse_move_event }
{
    _state = {
        .position = orientation.position,
        .forward  = orientation.forward,
        .up       = orientation.up,
    };

    set_perspective_proj(persp_params);
    update();
}

void PerspectiveCamera::subscribe_to_input() {
    _keyboard_events.subscribe();
    _mouse_move_events.subscribe();
}

void PerspectiveCamera::unsubscribe_from_input() {
    _keyboard_events.unsubscribe();
    _mouse_move_events.unsubscribe();
}

// =============================================================================
void PerspectiveCamera::update() {
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

    auto move_speed = _config.move_speed * TimeKeeper::delta_time();
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
void PerspectiveCamera::keyboard_event(KeyboardEvent const &event) {
    auto const key_state =
        (event.event_type == KeyboardEventType::KEY_PRESS) ? true : false;

    switch(event.event_code) {
        case BTX_KB_W          : _kb.w      = key_state; break;
        case BTX_KB_A          : _kb.a      = key_state; break;
        case BTX_KB_S          : _kb.s      = key_state; break;
        case BTX_KB_D          : _kb.d      = key_state; break;
        case BTX_KB_LEFT_CTRL  : _kb.lctrl  = key_state; break;
        case BTX_KB_SPACE      : _kb.space  = key_state; break;
        case BTX_KB_LEFT_SHIFT : _kb.lshift = key_state; break;
        default: break;
    }
}

// =============================================================================
void PerspectiveCamera::mouse_move_event(MouseMoveEvent const &event) {
    _state.pitch += static_cast<float>(-event.y_offset) * _config.look_speed;
    _state.yaw   += static_cast<float>(event.x_offset) * _config.look_speed;

    if(_state.pitch > 89.0f)       { _state.pitch = 89.0f;  }
    else if(_state.pitch < -89.0f) { _state.pitch = -89.0f; }

    if(_state.yaw > 360.0f)    { _state.yaw -= 360.0f; }
    else if(_state.yaw < 0.0f) { _state.yaw += 360.0f; }
}

// =============================================================================
void
PerspectiveCamera::set_perspective_proj(PerspectiveParams const &persp_params) {
    _proj_matrix = math::persp_proj_rh_zo(
        persp_params.vfov_degrees,
        persp_params.aspect_ratio,
        persp_params.near_plane,
        persp_params.far_plane
    );
}

// =============================================================================
void PerspectiveCamera::_process_events() {
    _keyboard_events.process_queue();
    _mouse_move_events.process_queue();
}

} // namespace btx