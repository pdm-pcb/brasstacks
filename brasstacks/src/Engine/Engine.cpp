#include "brasstacks/System/pch.hpp"
#include "brasstacks/Engine/Engine.hpp"

#include "brasstacks/Application/TargetWindow.hpp"
#include "brasstacks/Engine/RenderContext.hpp"
#include "brasstacks/Engine/RenderConfig.hpp"

#include "brasstacks/System/Input/KeyCodes.hpp"
#include "brasstacks/Events/Event.hpp"
#include "brasstacks/Events/KeyboardEvent.hpp"

namespace btx {

void Engine::on_event(Event &event) {
    switch(event.type()) {
        case EventType::WindowClosed:
            BTX_ENGINE_TRACE("Engine received WindowClosed");
            _render_context->shutdown();
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

void Engine::renderer() {
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

    _render_context->run();
}

Engine::Engine() :
    _render_context { RenderContext::create() },
    _clear_color    { 0.25f, 0.35f, 0.15f, 1.0f }
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
}

} // namespace btx