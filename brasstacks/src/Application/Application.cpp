#include "brasstacks/System/pch.hpp"
#include "brasstacks/Application/Application.hpp"

#include "brasstacks/Application/ConfigWindow.hpp"
#include "brasstacks/Application/TargetWindow.hpp"
#include "brasstacks/Engine/Engine.hpp"

namespace btx {

void Application::on_event(Event &event) {
    if(event.type() == EventType::WindowClosed) {
        BTX_ENGINE_TRACE("Application received WindowClosed");
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
    if(!_running) {
        return;
    }

    TargetWindow *target_window  = TargetWindow::create();
    target_window->init();
    target_window->subscribe_to(this, EventType::WindowClosed);
    TargetWindow::set_current(target_window);

    Engine *engine = new Engine;
    std::thread render_thread(&Engine::render_thread, engine);
    std::thread update_thread(&Engine::update_thread, engine);

    target_window->run();
    render_thread.join();
    update_thread.join();

    delete engine;
    delete target_window;
}

Application::Application() :
    _running { true }
{
    BTX_ENGINE_TRACE("Application constructor");
}

Application::~Application() {
    BTX_ENGINE_TRACE("Application destructor");
}

} // namespace btx