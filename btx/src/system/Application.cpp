#include "brasstacks/system/Application.hpp"

#include "brasstacks/events/EventBroker.hpp"
#include "brasstacks/events/keyboard_events.hpp"

#include "brasstacks/system/TargetWindow.hpp"
#include "brasstacks/renderer/Renderer.hpp"

namespace btx {

// =============================================================================
void Application::run() {
    this->init();

    while(_running) {
        _renderer->acquire_next_frame();
        _renderer->record_commands();
        _renderer->submit_commands();
        _renderer->present_image();
        _target_window->message_loop();
    }

    this->shutdown();
}

// =============================================================================
void Application::on_key_release(KeyReleaseEvent const &event) {
    if(event.code == BTX_KB_ESCAPE) {
        _running = false;
    }
}

// =============================================================================
Application::Application(std::string_view const app_name) :
    _running { true }
{
    ConsoleLog::init();

    EventBroker::init();
    EventBroker::subscribe<KeyReleaseEvent>(
        this,
        &Application::on_key_release
    );

    _target_window = TargetWindow::create(app_name);
    _renderer = new Renderer(*_target_window);
}

Application::~Application() {
    // Kill the event broker first so nothing unexpected pops up during
    // shutdown
    EventBroker::shutdown();

    delete _renderer;
    delete _target_window;
}

} // namespace btx