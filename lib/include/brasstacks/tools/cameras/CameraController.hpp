#ifndef BRASSTACKS_TOOLS_CAMERAS_CAMERACONTROLLER_HPP
#define BRASSTACKS_TOOLS_CAMERAS_CAMERACONTROLLER_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/events/EventQueue.hpp"
#include "brasstacks/events/keyboard_events.hpp"
#include "brasstacks/events/mouse_events.hpp"

namespace btx {

class Renderer;
class PerspectiveCamera;

class vkBuffer;
class vkDescriptorSetLayout;
class vkDescriptorSet;

class CameraController {
public:
    explicit CameraController(Renderer const &renderer);
    ~CameraController();

    CameraController() = delete;

    CameraController(CameraController &&) = delete;
    CameraController(CameraController const &) = delete;

    CameraController & operator=(CameraController &&) = delete;
    CameraController & operator=(CameraController const &) = delete;

private:
    Renderer const &_renderer;

    PerspectiveCamera *_perspective_camera;

    std::vector<vkBuffer *>        _camera_ubos;
    vkDescriptorSetLayout         *_camera_ubo_layout;
    std::vector<vkDescriptorSet *> _camera_ubo_sets;
};

} // namespace btx

#endif // BRASSTACKS_TOOLS_CAMERAS_CAMERACONTROLLER_HPP