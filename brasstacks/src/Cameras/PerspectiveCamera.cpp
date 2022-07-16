#include "brasstacks/System/pch.hpp"
#include "brasstacks/Cameras/PerspectiveCamera.hpp"

namespace btx {

void PerspectiveCamera::orient(const glm::vec3 &position,
                               const glm::vec3 &target,
                               const glm::vec3 &up) {
    set_viewmat(glm::lookAt(position, target, up));
    set_position(glm::vec4(position, 1.0f));
}

void PerspectiveCamera::calculate_proj(const float fov, const float aspect,
                                       const float near_plane,
                                       const float far_plane) {
    set_projmat(glm::perspective(fov, aspect, near_plane, far_plane));
}

PerspectiveCamera::PerspectiveCamera(const float fov, const float aspect,
                                     const float near_plane,
                                     const float far_plane) :
    Camera(glm::mat4(1.0f), glm::mat4(1.0f))
{
    calculate_proj(fov, aspect, near_plane, far_plane);
}

PerspectiveCamera::PerspectiveCamera() :
    Camera(glm::mat4(1.0f), glm::mat4(1.0f))
{ }

} // namespace btx