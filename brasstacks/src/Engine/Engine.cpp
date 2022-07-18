#include "brasstacks/System/pch.hpp"
#include "brasstacks/Engine/Engine.hpp"

#include "brasstacks/Application/TargetWindow.hpp"
#include "brasstacks/Engine/RenderContext.hpp"
#include "brasstacks/Engine/RenderConfig.hpp"

#include "brasstacks/System/Input/KeyCodes.hpp"
#include "brasstacks/Events/Event.hpp"
#include "brasstacks/Events/KeyboardEvent.hpp"


#include "brasstacks/Shaders/ShaderFlatColor.hpp"
#include "brasstacks/Meshes/MeshFlatColor.hpp"
#include "brasstacks/Engine/RenderQueue.hpp"
#include "brasstacks/Cameras/PerspectiveCamera.hpp"

#include "brasstacks/ECS/Scene.hpp"
#include "brasstacks/ECS/Components/TransformComponent.hpp"
#include "brasstacks/ECS/Components/MovementComponent.hpp"
#include "brasstacks/ECS/Components/CameraComponent.hpp"
#include "brasstacks/ECS/Components/CubeComponent.hpp"

namespace btx {

std::uint32_t component_counter = 0;

bool w = false;
bool a = false;
bool s = false;
bool d = false;

void Engine::on_event(Event &event) {
    switch(event.type()) {
        case EventType::WindowClosed:
            BTX_ENGINE_TRACE("Engine received WindowClosed");
            RenderQueue::shutdown();
            _render_context->shutdown();
            _render_thread_running.store(false);
            _update_thread_running.store(false);
            break;

        case EventType::KeyPressed:
        {
            auto *key_event = reinterpret_cast<KeyPressedEvent *>(&event);
            switch(key_event->key()) {
                case KB_W: w = true; break;
                case KB_A: a = true; break;
                case KB_S: s = true; break;
                case KB_D: d = true; break;
            }
            break;
        }

        case EventType::KeyReleased:
        {
            auto *key_event = reinterpret_cast<KeyReleasedEvent *>(&event);
            switch(key_event->key()) {
                case KB_W: w = false; break;
                case KB_A: a = false; break;
                case KB_S: s = false; break;
                case KB_D: d = false; break;
            }
            break;
        }

        default: break;
    }
}

void update_cam(Scene &scene, const float frame_delta) {
    for(const auto ent : SceneView<CameraComponent, TransformComponent,
                                   MovementComponent>(scene))
    {
        TransformComponent* transform = scene.get<TransformComponent>(ent);
        MovementComponent*  movement  = scene.get<MovementComponent>(ent);

        glm::vec3 forward(0.0f, 0.0f, -1.0f);
        glm::vec3 right(1.0f, 0.0f, 0.0f);

        movement->speed   = 0.0f;
        movement->heading = { 0.0f, 0.0f, 0.0f };

        if(w) {
            movement->speed = 1.5f * frame_delta;
            movement->heading += forward;
        }
        else if(s) {
            movement->speed = 1.5f * frame_delta;
            movement->heading += -forward;
        }
    
        if(d) {
            movement->speed = 1.5f * frame_delta;
            movement->heading += right;
        }
        else if(a) {
            movement->speed = 1.5f * frame_delta;
            movement->heading += -right;
        }

        transform->position += movement->heading * movement->speed;
    }
}

void update_cubes(Scene scene, const float frame_delta) {
    for(const auto ent : SceneView<CubeComponent, TransformComponent>(scene))
    {
        TransformComponent* transform = scene.get<TransformComponent>(ent);

        auto rot_factor = 0.436332f * Clock::runtime();

        auto x = glm::angleAxis(0.5f * rot_factor, glm::vec3(1.0f, 0.0f, 0.0f));
        auto y = glm::angleAxis(rot_factor, glm::vec3(0.0f, 1.0f, 0.0f));
        auto z = glm::angleAxis(2.0f * rot_factor, glm::vec3(0.0f, 0.0f, 1.0f));

        transform->rotation = x * y * z;

        if(transform->scale.x <= 0.05f) continue;
        transform->scale -= glm::vec3(0.001f);
    }
}

void Engine::update_thread() {
    {
        std::unique_lock<std::mutex> lock(_thread_startup);
        _render_thread_ready.wait(
            lock,
            [this]{ return _render_thread_running.load(); }
        );
    }



    Scene scene;

    EntityID camera = scene.new_entity();
    EntityID cube01 = scene.new_entity();
    EntityID cube02 = scene.new_entity();

    scene.assign<CameraComponent>(camera);
    scene.assign<MovementComponent>(camera);
    scene.assign<CubeComponent>(cube01);
    scene.assign<CubeComponent>(cube02);

    TransformComponent *camera_tc = scene.assign<TransformComponent>(camera);
    camera_tc->position = { 0.0f, 0.0f, 2.0f       };
    camera_tc->rotation = { 1.0f, 0.0f, 0.0f, 0.0f };
    camera_tc->scale    = { 1.0f, 1.0f, 1.0f       };

    TransformComponent *cube01_tc = scene.assign<TransformComponent>(cube01);
    cube01_tc->position = { 0.75f, 0.25f, 0.5f     };
    cube01_tc->rotation = { 1.0f, 0.0f, 0.0f, 0.0f };
    cube01_tc->scale    = { 1.0, 1.0f, 1.0f        };

    TransformComponent *cube02_tc = scene.assign<TransformComponent>(cube02);
    cube02_tc->position = { -0.75f, -0.25f, -0.5f  };
    cube02_tc->rotation = { 1.0f, 0.0f, 0.0f, 0.0f };
    cube02_tc->scale    = { 1.0f, 1.0f, 1.0f       };






    _update_thread_running.store(true);
    while(_update_thread_running) {
        Clock::update_tick();
        Clock::process_time();

        update_cam(scene, Clock::frame_delta());
        update_cubes(scene, Clock::frame_delta());

        _camera->orient(
            camera_tc->position,
            camera_tc->position + glm::vec3(0.0f, 0.0f, -2.0f)
        );

        RenderQueue::begin_scene();

            _mesh01->set_world_mat(
                glm::translate(glm::mat4(1.0f), cube01_tc->position) *
                glm::mat4(cube01_tc->rotation) *
                glm::scale(cube01_tc->scale)
            );

            _mesh02->set_world_mat(
                glm::translate(glm::mat4(1.0f), cube02_tc->position) *
                glm::mat4(cube02_tc->rotation) *
                glm::scale(cube02_tc->scale)
            );

            RenderQueue::submit(_shader, _mesh01);            
            RenderQueue::submit(_shader, _mesh02);

        RenderQueue::end_scene();

        Clock::update_tock();
    }
}

void Engine::render_thread() {
    RenderQueue::init();

    _render_context->init(_camera);
    _render_context->set_clear_color(
        _clear_color.r,
        _clear_color.g,
        _clear_color.b,
        _clear_color.a
    );

    if(RenderConfig::vsync_on) {
        _render_context->set_swap_interval(1);
    }

    _shader = new ShaderFlatColor;
    _mesh01 = new MeshFlatColor(Mesh::Primitives::Cube);
    _mesh02 = new MeshFlatColor(Mesh::Primitives::Cube);

    _render_thread_running.store(true);
    _render_thread_ready.notify_one();

    _render_context->run();

    delete _shader;
    delete _mesh01;
    delete _mesh02;
}

Engine::Engine() :
    _render_thread_running { false },
    _update_thread_running { false },
    _render_context { RenderContext::create() },
    _clear_color    { 0.11f, 0.11f, 0.11f, 1.0f },
    _shader { nullptr },
    _mesh01 { nullptr },
    _mesh02 { nullptr },
    _camera { 
        new PerspectiveCamera(
            math::pi_over_four,
            static_cast<float>(RenderConfig::window_x_res) /
            static_cast<float>(RenderConfig::window_y_res)
        )
    }

{
    TargetWindow::current()->subscribe_to(this, EventType::WindowClosed);
    TargetWindow::current()->subscribe_to(this, EventType::KeyPressed);
    TargetWindow::current()->subscribe_to(this, EventType::KeyReleased);
    TargetWindow::current()->subscribe_to(this, EventType::MouseMoved);
    TargetWindow::current()->subscribe_to(this, EventType::MouseButtonPressed);
    TargetWindow::current()->subscribe_to(this, EventType::MouseButtonReleased);

    _camera->orient(
        { 0.0f, 0.0f, 2.0f },
        { 0.0f, 0.0f, 0.0f }
    );
}

Engine::~Engine() {
    TargetWindow::current()->unsubscribe(this, EventType::WindowClosed);
    TargetWindow::current()->unsubscribe(this, EventType::KeyPressed);
    TargetWindow::current()->unsubscribe(this, EventType::KeyReleased);
    TargetWindow::current()->unsubscribe(this, EventType::MouseMoved);
    TargetWindow::current()->unsubscribe(this, EventType::MouseButtonPressed);
    TargetWindow::current()->unsubscribe(this, EventType::MouseButtonReleased);

    delete _render_context;
    delete _camera;
}

} // namespace btx