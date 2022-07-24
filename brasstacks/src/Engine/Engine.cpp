#include "brasstacks/System/pch.hpp"
#include "brasstacks/Engine/Engine.hpp"

#include "brasstacks/Application/TargetWindow.hpp"
#include "brasstacks/Engine/RenderContext.hpp"
#include "brasstacks/Engine/RenderConfig.hpp"
#include "brasstacks/Engine/RenderQueue.hpp"
#include "brasstacks/Engine/LayerStack.hpp"

#include "brasstacks/System/Input/KeyCodes.hpp"
#include "brasstacks/Events/Event.hpp"
#include "brasstacks/Events/KeyboardEvent.hpp"

#include "brasstacks/ECS/ECSView.hpp"

namespace btx {

extern Layer * create_layer();

void Engine::on_event(Event &event) {
    switch(event.type()) {
        case EventType::WindowClosed:
            BTX_ENGINE_TRACE("Engine received WindowClosed");
            RenderQueue::shutdown();
            _render_thread_running.store(false);
            _update_thread_running.store(false);
            break;

        default: break;
    }

    LayerStack::on_event(event);
}

void Engine::update_thread() {

    LayerStack::init();
    _wait_for_render_thread();

    auto *user_layer = create_layer();
    LayerStack::push_layer(user_layer);

    _update_thread_running.store(true);
    while(_update_thread_running) {
        RenderQueue::begin_scene();
        Clock::update_tick();

            LayerStack::update_layers();

            for(const auto id : ECSView<cRender>(*_ecs)) {
                auto render = _ecs->get<cRender>(id);
                RenderQueue::submit(render->shader, id);
            }

        RenderQueue::end_scene();
        Clock::update_tock();
    }

    LayerStack::shutdown();
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

    load_resources();

    _render_thread_running.store(true);
    _render_thread_ready.notify_all();

    while(_render_thread_running) {
        _render_context->run();
    }

    MeshLibrary::shutdown();
    TextureLibrary::shutdown();
    ShaderLibrary::shutdown();

    _render_context->shutdown();
}

void Engine::_wait_for_render_thread() {
    std::unique_lock<std::mutex> lock(_thread_startup);
    _render_thread_ready.wait(
        lock,
        [this]{ return _render_thread_running.load(); }
    );
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
    RenderContext::set_active(_render_context);
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