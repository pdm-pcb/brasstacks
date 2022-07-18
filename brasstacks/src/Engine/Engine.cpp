#include "brasstacks/System/pch.hpp"
#include "brasstacks/Engine/Engine.hpp"

#include "brasstacks/Application/TargetWindow.hpp"
#include "brasstacks/Engine/RenderContext.hpp"
#include "brasstacks/Engine/RenderConfig.hpp"
#include "brasstacks/Engine/RenderQueue.hpp"

#include "brasstacks/System/Input/KeyCodes.hpp"
#include "brasstacks/Events/Event.hpp"
#include "brasstacks/Events/KeyboardEvent.hpp"

#include "brasstacks/ECS/ECS.hpp"
#include "brasstacks/ECS/ECSView.hpp"
#include "brasstacks/ECS/Systems/CameraSystem.hpp"
#include "brasstacks/ECS/Systems/CubeSystem.hpp"


#include "brasstacks/Shaders/ShaderFlatColor.hpp"
#include "brasstacks/Meshes/MeshFlatColor.hpp"
#include "brasstacks/Cameras/PerspectiveCamera.hpp"

namespace btx {

std::atomic<bool> w = false;
std::atomic<bool> a = false;
std::atomic<bool> s = false;
std::atomic<bool> d = false;

void Engine::on_event(Event &event) {
    switch(event.type()) {
        case EventType::WindowClosed:
            BTX_ENGINE_TRACE("Engine received WindowClosed");
            RenderQueue::shutdown();
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

void Engine::update_thread() {
    {
        std::unique_lock<std::mutex> lock(_thread_startup);
        _render_thread_ready.wait(
            lock,
            [this]{ return _render_thread_running.load(); }
        );
    }



    Entity::ID camera = _ecs->new_entity();
    Entity::ID cube01 = _ecs->new_entity();
    Entity::ID cube02 = _ecs->new_entity();

    _ecs->assign<CameraComponent>(camera);
    _ecs->assign<MovementComponent>(camera);
    _ecs->assign<CubeComponent>(cube01);
    _ecs->assign<CubeComponent>(cube02);

    auto camera_tc = _ecs->assign<TransformComponent>(camera);
    camera_tc->position = { 0.0f, 0.0f, 2.0f       };
    camera_tc->rotation = { 1.0f, 0.0f, 0.0f, 0.0f };
    camera_tc->scale    = { 1.0f, 1.0f, 1.0f       };

    auto cube01_tc = _ecs->assign<TransformComponent>(cube01);
    cube01_tc->position = { 0.75f, 0.25f, 0.5f     };
    cube01_tc->rotation = { 1.0f, 0.0f, 0.0f, 0.0f };
    cube01_tc->scale    = { 1.0, 1.0f, 1.0f        };

    auto cube02_tc = _ecs->assign<TransformComponent>(cube02);
    cube02_tc->position = { -0.75f, -0.25f, -0.5f  };
    cube02_tc->rotation = { 1.0f, 0.0f, 0.0f, 0.0f };
    cube02_tc->scale    = { 1.0f, 1.0f, 1.0f       };




    _update_thread_running.store(true);
    while(_update_thread_running) {
        RenderQueue::begin_scene();
        Clock::update_tick();
        
            Clock::process_time();

            CameraSystem::update(_ecs, { w, a, s, d }, Clock::frame_delta());
            _camera->orient(
                camera_tc->position,
                camera_tc->position + glm::vec3(0.0f, 0.0f, -2.0f)
            );

            CubeSystem::update(_ecs, Clock::frame_delta());
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

    while(_render_thread_running) {
        _render_context->run();
    }
    
    _render_context->shutdown();

    delete _shader;
    delete _mesh01;
    delete _mesh02;
}

Engine::Engine() :
    _render_thread_running { false },
    _update_thread_running { false },
    _render_context { RenderContext::create() },
    _clear_color    { 0.11f, 0.11f, 0.11f, 1.0f },
    _ecs    { new ECS },
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
    delete _ecs;
}

} // namespace btx