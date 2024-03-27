#include "brasstacks/brasstacks.hpp"
#include "brasstacks/tools/cameras/CameraController.hpp"

#include "brasstacks/core/Renderer.hpp"
#include "brasstacks/tools/cameras/PerspectiveCamera.hpp"

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
}

// =============================================================================
CameraController::~CameraController() {
    delete _perspective_camera;
}

} // namespace btx