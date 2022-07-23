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
#include "brasstacks/Cameras/CameraBag.hpp"

namespace btx {

std::atomic<bool> w = false;
std::atomic<bool> a = false;
std::atomic<bool> s = false;
std::atomic<bool> d = false;
std::atomic<bool> shift = false;

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
                case KB_W:      w     = true; break;
                case KB_A:      a     = true; break;
                case KB_S:      s     = true; break;
                case KB_D:      d     = true; break;
                case KB_LSHIFT: shift = true; break;
            }
            break;
        }

        case EventType::KeyReleased:
        {
            auto *key_event = reinterpret_cast<KeyReleasedEvent *>(&event);
            switch(key_event->key()) {
                case KB_W:      w     = false; break;
                case KB_A:      a     = false; break;
                case KB_S:      s     = false; break;
                case KB_D:      d     = false; break;
                case KB_LSHIFT: shift = false; break;
            }
            break;
        }

        case EventType::MouseMoved:
        {
            auto [ x_offset, y_offset ] =
                reinterpret_cast<MouseMovedEvent *>(&event)->offset();

            auto camera = _ecs->get<cCamera>(CameraBag::get_active());
            camera->pitch -= 0.00125f * y_offset;
            camera->yaw   += 0.00125f * x_offset;

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

    _update_thread_running.store(true);
    while(_update_thread_running) {
        RenderQueue::begin_scene();
        Clock::update_tick();

            CameraSystem::update({ w, a, s, d, shift }, Clock::frame_delta());

            user_update_code();

            for(const auto id : ECSView<cRender>(*_ecs)) {
                auto render = _ecs->get<cRender>(id);
                RenderQueue::submit(render->shader, id);
            }

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

    MeshLibrary::init();
    TextureLibrary::init();
    ShaderLibrary::init();

    load_user_resources();

    _render_thread_running.store(true);
    _render_thread_ready.notify_one();

    while(_render_thread_running) {
        _render_context->run();
    }

    MeshLibrary::shutdown();
    TextureLibrary::shutdown();
    ShaderLibrary::shutdown();

    _render_context->shutdown();
}

Engine::Engine() :
    _render_thread_running { false },
    _update_thread_running { false },
    _render_context { RenderContext::create() },
    _ecs { new ECS }

{
    TargetWindow::current()->subscribe_to(this, EventType::WindowClosed);
    TargetWindow::current()->subscribe_to(this, EventType::KeyPressed);
    TargetWindow::current()->subscribe_to(this, EventType::KeyReleased);
    TargetWindow::current()->subscribe_to(this, EventType::MouseMoved);
    TargetWindow::current()->subscribe_to(this, EventType::MouseButtonPressed);
    TargetWindow::current()->subscribe_to(this, EventType::MouseButtonReleased);

    ECS::set_active(_ecs);

    Entity::ID camera = _ecs->new_entity();
    _ecs->assign<cMove>(camera);

    auto camera_tc = _ecs->assign<cTransform>(camera);
    camera_tc->position  = { 0.0f, 0.0f, 2.0f };

    auto camera_cc = _ecs->assign<cCamera>(camera);
    camera_cc->view_matrix = glm::lookAt(
        camera_tc->position,
        camera_cc->forward,
        camera_cc->up
    );
    camera_cc->proj_matrix = glm::perspective(
        math::pi_over_four,
        static_cast<float>(RenderConfig::window_x_res) /
        static_cast<float>(RenderConfig::window_y_res),
        RenderConfig::near_clip, RenderConfig::far_clip
    );

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