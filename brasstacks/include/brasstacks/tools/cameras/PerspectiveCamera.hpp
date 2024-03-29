#ifndef BRASSTACKS_TOOLS_CAMERAS_PERSPECTIVECAMERA_HPP
#define BRASSTACKS_TOOLS_CAMERAS_PERSPECTIVECAMERA_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/math/math.hpp"

namespace btx {

struct KeyboardEvent;
struct MouseMoveEvent;

class PerspectiveCamera final {
public:
    struct Orientation {
        math::Vec3 position = math::Vec3::zero;
        math::Vec3 forward = math::Vec3::zero;
        math::Vec3 up = math::Vec3::zero;

        float pitch = 0.0f;
        float yaw = -90.0f;
    };

    struct PerspectiveParams {
        float vfov_degrees = 45.0f;
        float aspect_ratio = (16.0f / 9.0f);
        float near_plane = 0.1f;
        float far_plane = 1000.0f;
    };

    PerspectiveCamera(Orientation const &orientation,
                      PerspectiveParams const &persp_params);
    ~PerspectiveCamera() = default;

    void subscribe_to_input();
    void unsubscribe_from_input();
    void update();

    void keyboard_event(KeyboardEvent const &event);
    void mouse_move_event(MouseMoveEvent const &event);

    void set_perspective_proj(PerspectiveParams const &persp_params);

    inline auto const & view_matrix() const { return _view_matrix; }
    inline auto const & proj_matrix() const { return _proj_matrix; }

    PerspectiveCamera() = delete;

    PerspectiveCamera(PerspectiveCamera &&) = delete;
    PerspectiveCamera(PerspectiveCamera const &) = delete;

    PerspectiveCamera & operator=(PerspectiveCamera &&) = delete;
    PerspectiveCamera & operator=(PerspectiveCamera const &) = delete;

private:
    math::Mat4 _view_matrix;
    math::Mat4 _proj_matrix;

    struct CameraConfig {
        float move_speed = 1.0f;
        float look_speed = 0.075f;
    } _config;

    struct KeyboardState {
        bool w      = false;
        bool a      = false;
        bool s      = false;
        bool d      = false;
        bool lctrl  = false;
        bool space  = false;
        bool lshift = false;
    } _kb;

    struct CameraState {
        math::Vec3 position = math::Vec3::unit_z;
        math::Vec3 forward  = -math::Vec3::unit_z;
        math::Vec3 side     = math::Vec3::unit_x;
        math::Vec3 up       = math::Vec3::unit_y;

        float pitch = 0.0f;
        float yaw   = -90.0f;
    } _state;

    EventQueue<KeyboardEvent>   _keyboard_events;
    EventQueue<MouseMoveEvent>  _mouse_move_events;

    void _process_events();
};

} // namespace btx

#endif // BRASSTACKS_TOOLS_CAMERAS_PERSPECTIVECAMERA_HPP