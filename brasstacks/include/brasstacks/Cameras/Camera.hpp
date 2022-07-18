#ifndef BRASSTACKS_CAMERAS_CAMERA_HPP
#define BRASSTACKS_CAMERAS_CAMERA_HPP

#include "brasstacks/System/glm.hpp"

namespace btx {

class Camera {
public:
    const glm::mat4& view_matrix()       const { return _view_matrix;       }
    const glm::mat4& projection_matrix() const { return _projection_matrix; }
    const glm::vec4  position()          const { return _position;          }

    void set_viewmat(const glm::mat4 &view) { _view_matrix = view;       }
    void set_projmat(const glm::mat4 &proj) { _projection_matrix = proj; }
    void set_position(const glm::vec4 &pos) { _position = pos;           }

    Camera(const Camera &) = delete;
    Camera(Camera &&)      = delete;

    Camera& operator=(const Camera &) = delete;
    Camera& operator=(Camera &&)      = delete;

protected:
    Camera(glm::mat4 view, glm::mat4 projection) :
        _view_matrix { view },
        _projection_matrix { projection },
        _position { 0.0f, 0.0f, 0.0f, 1.0f }
    { }

    Camera()  = default;
    ~Camera() = default;

private:
    glm::mat4 _view_matrix;
    glm::mat4 _projection_matrix;
    glm::vec4 _position;
};

} // namespace btx

#endif // BRASSTACKS_CAMERAS_CAMERA_HPP