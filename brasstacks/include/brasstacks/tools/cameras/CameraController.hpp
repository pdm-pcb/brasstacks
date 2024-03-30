#ifndef BRASSTACKS_TOOLS_CAMERAS_CAMERACONTROLLER_HPP
#define BRASSTACKS_TOOLS_CAMERAS_CAMERACONTROLLER_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/events/EventQueue.hpp"
#include "brasstacks/events/keyboard_events.hpp"
#include "brasstacks/events/mouse_events.hpp"
#include "brasstacks/tools/cameras/PerspectiveCamera.hpp"

namespace btx {

class vkBuffer;
class vkDescriptorSetLayout;
class vkDescriptorSet;

class CameraController {
public:
    static void init();
    static void shutdown();

    static inline void activate_camera() {
        _perspective_camera->subscribe_to_input();
    }
    static inline void deactivate_camera() {
        _perspective_camera->unsubscribe_from_input();
    }
    static inline void update_camera() {
        _perspective_camera->update();
    }

    static void update_perspective();

    static void update_ubo();

    static inline auto & camera_ubo_layout() { return *_camera_ubo_layout; }
    static inline auto & camera_ubo_set() {
        return *_camera_ubo_sets[Renderer::image_index()];
    }

    CameraController() = delete;
    ~CameraController() = delete;

    CameraController(CameraController &&) = delete;
    CameraController(CameraController const &) = delete;

    CameraController & operator=(CameraController &&) = delete;
    CameraController & operator=(CameraController const &) = delete;

private:
    static PerspectiveCamera *_perspective_camera;

    static std::vector<vkBuffer *>        _camera_ubos;
    static vkDescriptorSetLayout         *_camera_ubo_layout;
    static std::vector<vkDescriptorSet *> _camera_ubo_sets;
};

} // namespace btx

#endif // BRASSTACKS_TOOLS_CAMERAS_CAMERACONTROLLER_HPP