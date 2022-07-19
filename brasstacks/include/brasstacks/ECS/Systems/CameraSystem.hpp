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

private:
    static const glm::vec3 _forward;
    static const glm::vec3 _right;
    static const glm::vec3 _up;
    static const glm::mat4 _ident;
};

} // namespace btx

#endif // BRASSTACKS_ECS_CAMERASYSTEM_HPP
