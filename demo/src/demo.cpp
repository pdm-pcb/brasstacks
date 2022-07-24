#include "brasstacks/brasstacks.hpp"

#include "cCube.hpp"
#include "CameraSystem.hpp"
#include "CubeSystem.hpp"

void add_terrain();
void add_cube();
void add_camera();

std::atomic<bool> w = false;
std::atomic<bool> a = false;
std::atomic<bool> s = false;
std::atomic<bool> d = false;
std::atomic<bool> shift = false;
std::atomic<bool> ctrl  = false;
std::atomic<bool> space = false;

class GameplayLayer : public btx::Layer {
    void on_attach() override {
        BTX_TRACE("Gameplay Layer attached.");
        add_camera();
        add_terrain();
        add_cube();
    }

    void on_detach() override {
        BTX_TRACE("Gameplay Layer detached.");
    }

    void update() override {
        CameraSystem::update(
            { w, a, s, d, shift, ctrl, space },
            btx::Clock::frame_delta()
        );
        CubeSystem::update(btx::Clock::frame_delta());
    }

    void on_event(btx::Event &event) override {
        switch(event.type()) {
            case btx::EventType::KeyPressed:
            {
                auto *key_event = reinterpret_cast<btx::KeyPressedEvent *>(&event);
                switch(key_event->key()) {
                    case btx::KB_W:      w     = true; break;
                    case btx::KB_A:      a     = true; break;
                    case btx::KB_S:      s     = true; break;
                    case btx::KB_D:      d     = true; break;
                    case btx::KB_LSHIFT: shift = true; break;
                    case btx::KB_LCTRL:  ctrl  = true; break;
                    case btx::KB_SPACE:  space = true; break;
                }
                break;
            }

            case btx::EventType::KeyReleased:
            {
                auto *key_event = reinterpret_cast<btx::KeyReleasedEvent *>(&event);
                switch(key_event->key()) {
                    case btx::KB_W:      w     = false; break;
                    case btx::KB_A:      a     = false; break;
                    case btx::KB_S:      s     = false; break;
                    case btx::KB_D:      d     = false; break;
                    case btx::KB_LSHIFT: shift = false; break;
                    case btx::KB_LCTRL:  ctrl  = false; break;
                    case btx::KB_SPACE:  space = false; break;
                }
                break;
            }

            case btx::EventType::MouseMoved:
            {
                auto [ x_offset, y_offset ] =
                    reinterpret_cast<btx::MouseMovedEvent *>(&event)->offset();

                auto ecs = btx::ECS::get_active();
                auto camera = ecs->get<btx::cCamera>(btx::CameraBag::get_active());
                camera->pitch -= 0.00125f * y_offset;
                camera->yaw   += 0.00125f * x_offset;

                break;
            }

            default: break;
        }
    }
};

btx::Layer * btx::create_layer() {
    return new GameplayLayer;
}