#ifndef BRASSTACKS_TOOLS_FPSCAMERA_HPP
#define BRASSTACKS_TOOLS_FPSCAMERA_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/math/math.hpp"

namespace btx {

class vkDevice;
class vkDescriptorPool;
class vkDescriptorSetLayout;
class vkDescriptorSet;
class vkBuffer;
class vkCmdBuffer;

struct KeyPressEvent;
struct KeyReleaseEvent;
struct MouseMoveEvent;

class FPSCamera final {
public:
    struct Orientation {
        math::Vec3 position = math::Vec3::zero;
        math::Vec3 forward = math::Vec3::zero;
        math::Vec3 up = math::Vec3::zero;

        float pitch = 0.0f;
        float yaw = -90.0f;
    };

    FPSCamera(vkDevice const &device, vkDescriptorPool const &descriptor_pool,
              size_t const image_count, Orientation const &orientation);
    ~FPSCamera();

    void update();
    void send_buffer_data(uint32_t const image_index);

    void on_key_press(KeyPressEvent const &event);
    void on_key_release(KeyReleaseEvent const &event);
    void on_mouse_move(MouseMoveEvent const &event);

    inline auto const & desc_layout() const { return *_camera_ubo_layout; }
    inline auto const & desc_set(uint32_t const image_index) const {
        return *_camera_ubo_sets[image_index];
    }

private:
    vkDevice const &_device;

    vkDescriptorSetLayout         *_camera_ubo_layout;
    std::vector<vkDescriptorSet *> _camera_ubo_sets;
    std::vector<vkBuffer *>        _camera_ubos;

    math::Mat4 _view_matrix;
    math::Mat4 _proj_matrix;

    struct CameraConfig {
        float move_speed   = 1.0f;
        float look_speed   = 0.075f;
        float vfov_degrees = 45.0f;
        float aspect_ratio = 16.0f / 9.0f;
        float near_plane   = 0.1f;
        float far_plane    = 1000.0f;
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

void _allocate_descriptors(vkDescriptorPool const &descriptor_pool,
                           size_t const image_count);

void _register_callbacks();
};

} // namespace btx

#endif // BRASSTACKS_TOOLS_FPSCAMERA_HPP