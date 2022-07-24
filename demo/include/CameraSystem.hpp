#pragma once

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
