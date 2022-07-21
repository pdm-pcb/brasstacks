#ifndef BRASSTACKS_ECS_CAMERASYSTEM_HPP
#define BRASSTACKS_ECS_CAMERASYSTEM_HPP

#include "brasstacks/System/glm.hpp"
#include "brasstacks/ECS/Entity.hpp"

namespace btx {

class ECS;

class CameraSystem final {
public:
    struct InputState {
        bool w;
        bool a;
        bool s;
        bool d;
    };

    static void update(ECS *ecs, const InputState &input,
                       const float frame_delta);

    CameraSystem() = delete;
};

} // namespace btx

#endif // BRASSTACKS_ECS_CAMERASYSTEM_HPP
