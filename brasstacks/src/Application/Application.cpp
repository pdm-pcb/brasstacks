#include "brasstacks/System/pch.hpp"
#include "brasstacks/Application/Application.hpp"

#include "brasstacks/Application/ConfigWindow.hpp"
#include "brasstacks/Engine/TargetWindow.hpp"
#include "brasstacks/Engine/RenderContext.hpp"

namespace btx {

void Application::on_event(Event &event) {
    if(event.type() == EventType::WindowClosed) {
        _running = false;
    }
}

void Application::configure(const char *conf_filename) {
    ConfigWindow *conf_window = ConfigWindow::create();

    conf_window->subscribe_to(this, EventType::WindowClosed);

    conf_window->run();
    delete conf_window;
}

void Application::run() {
    if(!_running) {     // TODO: Implement a messaging system for this
        return;
    }

    TargetWindow *target_window   = TargetWindow::create();
    RenderContext *render_context = RenderContext::create(target_window);

    target_window->subscribe_to(this, EventType::WindowClosed);

    ::glClearColor(0.25f, 0.5f, 0.15f, 1.0f);
    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    render_context->swap_buffers();

    target_window->run();
    render_context->shutdown();

    delete render_context;
    delete target_window;
}

Application::Application() :
    _running { true }
{
    BTX_ENGINE_TRACE("Here we goooooooooooooo~");
}

Application::~Application() {
    BTX_ENGINE_TRACE("There we weeeeeeeeeeeeeent~");
}

} // namespace btx