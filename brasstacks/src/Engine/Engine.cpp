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
#include "brasstacks/ECS/EntityRegistrar.hpp"
#include "brasstacks/ECS/Systems/TransformSystem.hpp"

namespace btx {

void Engine::on_event(Event &event) {
    switch(event.type()) {
        case EventType::WindowClosed:
            BTX_ENGINE_TRACE("Engine received WindowClosed");
            _render_context->shutdown();
            RenderQueue::shutdown();
            _render_thread_running.store(false);
            _update_thread_running.store(false);
            break;

        case EventType::KeyPressed:
        {
            auto *key_event = reinterpret_cast<KeyPressedEvent *>(&event);
            switch(key_event->key()) {
                case KB_A:
                    BTX_ENGINE_TRACE("A pressed");
                    break;
            }
            break;
        }

        case EventType::KeyReleased:
            break;

        default:
            BTX_ENGINE_WARN("Unknown event type received by Engine");
            break;
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

    EntityRegistrar::init();
    Entity cube_id01 = EntityRegistrar::create_entity();
    Entity cube_id02 = EntityRegistrar::create_entity();
    _transforms->add_entity(cube_id01);
    _transforms->add_entity(cube_id02);

    _update_thread_running.store(true);

    TransformComponent &transform01 = _transforms->get_transform(cube_id01);
    TransformComponent &transform02 = _transforms->get_transform(cube_id02);

    glm::vec3 starting01 { 0.75f,  0.25f,  0.5f};
    glm::vec3 starting02 {-0.75f, -0.25f, -0.5f};

    while(_update_thread_running) {
        Clock::update_tick();
        Clock::process_time();

        _transforms->update();

        RenderQueue::begin_scene();
            _mesh01->set_world_mat(
                glm::translate(glm::mat4(1.0f), starting01) *
                glm::mat4(transform01.rotation) *
                glm::scale(glm::mat4(1.0f), transform01.scale)
            );

            _mesh02->set_world_mat(
                glm::translate(glm::mat4(1.0f), starting02) *
                glm::mat4(transform02.rotation) *
                glm::scale(glm::mat4(1.0f), transform02.scale)
            );
            
            RenderQueue::submit(_shader, _mesh01);
            RenderQueue::submit(_shader, _mesh02);
        RenderQueue::end_scene();

        Clock::update_tock();
    }
}

void Engine::render_thread() {
    RenderQueue::init();

    _render_context->init();
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
    _shader     { nullptr },
    _mesh01     { nullptr },
    _mesh02     { nullptr },
    _transforms { new TransformSystem }

{
    TargetWindow::current()->subscribe_to(this, EventType::WindowClosed);
    TargetWindow::current()->subscribe_to(this, EventType::KeyPressed);
    TargetWindow::current()->subscribe_to(this, EventType::KeyReleased);
    TargetWindow::current()->subscribe_to(this, EventType::MouseMoved);
    TargetWindow::current()->subscribe_to(this, EventType::MouseButtonPressed);
    TargetWindow::current()->subscribe_to(this, EventType::MouseButtonReleased);
}

Engine::~Engine() {
    TargetWindow::current()->unsubscribe(this, EventType::WindowClosed);
    TargetWindow::current()->unsubscribe(this, EventType::KeyPressed);
    TargetWindow::current()->unsubscribe(this, EventType::KeyReleased);
    TargetWindow::current()->unsubscribe(this, EventType::MouseMoved);
    TargetWindow::current()->unsubscribe(this, EventType::MouseButtonPressed);
    TargetWindow::current()->unsubscribe(this, EventType::MouseButtonReleased);

    delete _render_context;
    
    delete _transforms;
}

} // namespace btx