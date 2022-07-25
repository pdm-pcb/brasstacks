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
    LayerStack::on_event(event);

    switch(event.type()) {
        case EventType::WindowClosed:
            BTX_ENGINE_TRACE("Engine received WindowClosed");
            _render_thread_running.store(false);
            _update_thread_running.store(false);
            RenderQueue::shutdown();
            break;

        default: break;
    }
}

void Engine::update_thread() {
    LayerStack::init();
    _wait_for_render_thread();

auto update_perf = Profiler::spawn("update");
auto submit_perf = Profiler::spawn("submit");

    auto *user_layer = create_layer();
    LayerStack::push_layer(user_layer);

    _update_thread_running.store(true);
    while(_update_thread_running) {
        RenderQueue::begin_scene();

        Clock::update_tick();
update_perf->start();
            Clock::update();
            LayerStack::update_layers();
update_perf->stop(0.0f);
submit_perf->start();
            for(const auto id : ECSView<cRender>(*_ecs)) {
                auto render = _ecs->get<cRender>(id);
                RenderQueue::submit(render->shader, id);
            }
submit_perf->stop(0.0f);
        RenderQueue::end_scene();
        Clock::update_tock();
    }

    LayerStack::shutdown();

delete update_perf;
delete submit_perf;
}

void Engine::render_thread() {
auto render_perf = Profiler::spawn("render");
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
render_perf->start();
        _render_context->run();
render_perf->stop(math::one_over_sixty_us);
    }

    MeshLibrary::shutdown();
    TextureLibrary::shutdown();
    ShaderLibrary::shutdown();

    _render_context->shutdown();

delete render_perf;
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
    BTX_ENGINE_TRACE("Engine constructor");

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
    BTX_ENGINE_TRACE("Engine destructor");

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