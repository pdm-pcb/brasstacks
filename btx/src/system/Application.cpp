#include "brasstacks/core.hpp"
#include "brasstacks/system/Application.hpp"

#include "brasstacks/events/EventBroker.hpp"
#include "brasstacks/events/keyboard_events.hpp"

#include "brasstacks/system/TargetWindow.hpp"
#include "brasstacks/rendering/Renderer.hpp"

namespace btx {

// =============================================================================
Application::Application(std::string_view const app_name) :
    _running { true }
{
    // Events can start firing potentially as soon as we let TargetWindow run
    // its message loop
    EventBroker::init();
    // Application uses key releases to controll its running state
    EventBroker::subscribe<KeyReleaseEvent>(this, &Application::on_key_release);

    // Instantiate the platform window and renderer backend
    _target_window = TargetWindow::create(app_name);
    _renderer = new Renderer(*_target_window);
}

Application::~Application() {
    // Kill Events first so nothing unexpected pops up during destruction
    EventBroker::shutdown();

    delete _renderer;
    delete _target_window;
}

// =============================================================================
void Application::run() {
    // Give the user a chance to bring up their data
    this->init();

    while(_running) {
        // Tick the clock
        Timekeeper::update();

        // Have the client do per-frame processing and submit draw calls
        this->update();

        Timekeeper::frame_start();

            // Let the renderer go to town on a frame
            _renderer->acquire_next_image();
            _renderer->record_commands();
            _renderer->submit_commands();
            _renderer->present_image();

        Timekeeper::frame_end();

        // Check for input and events from the OS
        _target_window->message_loop();
    }

    // Give the user a chance to clean up
    this->shutdown();
}

// =============================================================================
void Application::on_key_release(KeyReleaseEvent const &event) {
    // For now, a hard-coded check against Esc is how Application terminates
    if(event.code == BTX_KB_ESCAPE) {
        _running = false;
    }
}

// =============================================================================
void Application::request_draw() {
    // A simple passthrough until things get more configurable
    _renderer->request_draw();
}

} // namespace btx