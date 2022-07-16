#ifndef BRASSTACKS_CAMERAS_PERSPECTIVECAMERA_HPP
#define BRASSTACKS_CAMERAS_PERSPECTIVECAMERA_HPP

#include "brasstacks/Cameras/Camera.hpp"

namespace btx {

class PerspectiveCamera : public Camera {
public:
    void orient(const glm::vec3 &position, const glm::vec3 &target,
                const glm::vec3 &up = glm::vec3(0.0f, 1.0f, 0.0f));

    void calculate_proj(const float fov, const float aspect,
                        const float near_plane = 0.1f,
                        const float far_plane  = 25000.0f);

    PerspectiveCamera(const float fov, const float aspect,
                      const float near_plane = 0.1f,
                      const float far_plane  = 25000.0f);

    PerspectiveCamera();
    ~PerspectiveCamera() = default;

    PerspectiveCamera(const PerspectiveCamera &) = delete;
    PerspectiveCamera(PerspectiveCamera &&)      = delete;

    PerspectiveCamera & operator=(const PerspectiveCamera &) = delete;
    PerspectiveCamera & operator=(PerspectiveCamera &&)      = delete;
};

} // namespace btx

#endif // BRASSTACKS_CAMERAS_PERSPECTIVECAMERA_HPP