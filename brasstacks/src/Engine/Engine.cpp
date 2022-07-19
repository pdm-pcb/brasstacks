#include "brasstacks/System/pch.hpp"
#include "brasstacks/Engine/Engine.hpp"

#include "brasstacks/Application/TargetWindow.hpp"
#include "brasstacks/Engine/RenderContext.hpp"
#include "brasstacks/Engine/RenderConfig.hpp"
#include "brasstacks/Engine/RenderQueue.hpp"

#include "brasstacks/System/Input/KeyCodes.hpp"
#include "brasstacks/Events/Event.hpp"
#include "brasstacks/Events/KeyboardEvent.hpp"

#include "brasstacks/ECS/ECSView.hpp"
#include "brasstacks/ECS/Systems/CameraSystem.hpp"
#include "brasstacks/ECS/Systems/CubeSystem.hpp"
#include "brasstacks/Cameras/CameraBag.hpp"


#include "brasstacks/Shaders/ShaderFlatColor.hpp"
#include "brasstacks/Meshes/MeshFlatColor.hpp"

namespace btx {

std::atomic<bool> w = false;
std::atomic<bool> a = false;
std::atomic<bool> s = false;
std::atomic<bool> d = false;
std::atomic<bool> add_cube = false;

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

                case KB_SPACE: add_cube = true;
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

        case EventType::MouseMoved:
        {
            auto [ x_offset, y_offset ] =
                reinterpret_cast<MouseMovedEvent *>(&event)->offset();

            auto camera = _ecs->get<CameraComponent>(CameraBag::get_active());
            camera->pitch -= 0.00125f * y_offset;
            camera->yaw   += 0.00125f * x_offset;

            break;
        }

        default: break;
    }
}

void Engine::_add_cube() {
    Entity::ID new_cube = _ecs->new_entity();
    _ecs->assign<CubeComponent>(new_cube);

    auto cube_transform = _ecs->assign<TransformComponent>(new_cube);
    cube_transform->position = {
        _rng(_twister),
        _rng(_twister),
        -25.0f
    };

    auto cube_render = _ecs->assign<RenderComponent>(new_cube);
    MeshFlatColor::create(Mesh::Primitives::Cube, &cube_render->vb,
                          &cube_render->vertices, &cube_render->faces, 1.0f);
    cube_render->vertex_count = CUBE_VERTS;
    cube_render->face_count   = CUBE_FACES;
    cube_render->shader       = _shader;

    ++_cube_count;
    if(_cube_count % 10 == 0) {
        add_cube = false;
        BTX_ENGINE_TRACE(
            "Adding cube {}: {:.02f},{:.02f},{:.02f}",
            _cube_count,
            cube_transform->position.x,
            cube_transform->position.y,
            cube_transform->position.z
        );
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

    _update_thread_running.store(true);
    while(_update_thread_running) {
        RenderQueue::begin_scene();
        Clock::update_tick();

            CameraSystem::update(_ecs, { w, a, s, d }, Clock::frame_delta());
            CubeSystem::update(_ecs, Clock::frame_delta());
        
        RenderQueue::end_scene();
        Clock::update_tock();
    }
}

void Engine::render_thread() {
    RenderQueue::init();

    _render_context->init();
    _render_context->set_clear_color(0.11f, 0.11f, 0.11f, 1.0f);

    if(RenderConfig::vsync_on) {
        _render_context->set_swap_interval(1);
    }

    _shader = new ShaderFlatColor;

    _render_thread_running.store(true);
    _render_thread_ready.notify_one();

    while(_render_thread_running) {
        _render_context->run();

        if(add_cube) {
            _add_cube();
        }
    }
    
    _render_context->shutdown();

    delete _shader;
}

Engine::Engine() :
    _render_thread_running { false },
    _update_thread_running { false },
    _render_context { RenderContext::create() },
    _ecs    { new ECS },
    _shader { nullptr },
    _cube_count { 0 },
    _twister    { _rd() },
    _rng        { -10.0f, 10.0f }

{
    TargetWindow::current()->subscribe_to(this, EventType::WindowClosed);
    TargetWindow::current()->subscribe_to(this, EventType::KeyPressed);
    TargetWindow::current()->subscribe_to(this, EventType::KeyReleased);
    TargetWindow::current()->subscribe_to(this, EventType::MouseMoved);
    TargetWindow::current()->subscribe_to(this, EventType::MouseButtonPressed);
    TargetWindow::current()->subscribe_to(this, EventType::MouseButtonReleased);

    ECS::set_active(_ecs);

    Entity::ID camera = _ecs->new_entity();
    _ecs->assign<MovementComponent>(camera);

    auto camera_tc = _ecs->assign<TransformComponent>(camera);
    camera_tc->position = { 0.0f, 0.0f, 2.0f };

    auto camera_cc = _ecs->assign<CameraComponent>(camera);
    camera_cc->proj_matrix = glm::perspective(
        math::pi_over_four,
        static_cast<float>(RenderConfig::window_x_res) /
        static_cast<float>(RenderConfig::window_y_res),
        RenderConfig::near_clip, RenderConfig::far_clip
    );
    camera_cc->lookahead = { 0.0f, 0.0f, -2.0f };

    CameraBag::set_active(camera);
}

Engine::~Engine() {
    TargetWindow::current()->unsubscribe(this, EventType::WindowClosed);
    TargetWindow::current()->unsubscribe(this, EventType::KeyPressed);
    TargetWindow::current()->unsubscribe(this, EventType::KeyReleased);
    TargetWindow::current()->unsubscribe(this, EventType::MouseMoved);
    TargetWindow::current()->unsubscribe(this, EventType::MouseButtonPressed);
    TargetWindow::current()->unsubscribe(this, EventType::MouseButtonReleased);

    delete _render_context;
    delete _ecs;
}

} // namespace btx