#ifndef BRASSTACKS_CAMERAS_CAMERA_HPP
#define BRASSTACKS_CAMERAS_CAMERA_HPP

#include "brasstacks/ECS/Entity.hpp"

namespace btx {

class CameraBag {
public:
    static Entity::ID get_active()              { return _active; }
    static void set_active(const Entity::ID id) { _active = id;   }

    CameraBag() = delete;
    ~CameraBag() = default;

    CameraBag(const CameraBag &) = delete;
    CameraBag(CameraBag &&)      = delete;

    CameraBag& operator=(const CameraBag &) = delete;
    CameraBag& operator=(CameraBag &&)      = delete;

private:
    static Entity::ID _active;
};

} // namespace btx

#endif // BRASSTACKS_CAMERAS_CAMERA_HPP