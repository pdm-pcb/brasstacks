#ifndef BRASSTACKS_ECS_CAMERASYSTEM_HPP
#define BRASSTACKS_ECS_CAMERASYSTEM_HPP

#include "brasstacks/System/glm.hpp"
#include "brasstacks/ECS/Entity.hpp"

namespace btx {

class CameraSystem final {
public:
    struct InputState {
        bool w;
        bool a;
        bool s;
        bool d;
        bool shift;
        bool ctrl;
        bool space;
    };

    static void update(const InputState &input, const float frame_delta);

    CameraSystem() = delete;

private:
    static float _strafe_speed;
    static float _sprint_speed;
};

} // namespace btx

#endif // BRASSTACKS_ECS_CAMERASYSTEM_HPP
