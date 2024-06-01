#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/Application.hpp"

#include "brasstacks/core/TargetWindow.hpp"
#include "brasstacks/core/Renderer.hpp"

#include "brasstacks/tools/cameras/CameraController.hpp"

namespace btx {

// =============================================================================
Application::Application(std::string_view const app_name) :
    _running             { true },
    _window_events       { *this, &Application::_window_event },
    _keyboard_events     { *this, &Application::_keyboard_event }
{
    _window_events.subscribe();
    _keyboard_events.subscribe();

    TargetWindow::init(app_name);
    Renderer::init(this);
}

// =============================================================================
Application::~Application() {
    Renderer::shutdown();
    TargetWindow::shutdown();
}

// =============================================================================
void Application::run() {
    this->init();

    TargetWindow::capture_mouse();
    CameraController::activate_camera();
    TargetWindow::show();

    while(_running) {
        TimeKeeper::update_run_time();
        TargetWindow::poll_events();
        _process_events();
        CameraController::update_camera();
        this->update();
        Renderer::run();
    }

    TargetWindow::hide();
    Renderer::wait_device_idle();
    this->shutdown();
}

// =============================================================================
void Application::_process_events() {
    _window_events.process_queue();
    _keyboard_events.process_queue();
}

// =============================================================================
void Application::_window_event(WindowEvent const &event) {
    if(event.type == WindowEventType::WINDOW_CLOSE) {
        BTX_TRACE("Application received window close.");
        _running = false;
    }
}

// =============================================================================
void Application::_keyboard_event(KeyboardEvent const &event) {
    switch(event.code) {
        case BTX_KB_ESCAPE:
            if(event.type == KeyboardEventType::KEY_RELEASE) {
                BTX_TRACE("AppStateMenu publishing window close event");
                EventBus::publish(WindowEvent(WindowEventType::WINDOW_CLOSE));
            }
            break;
    }
}

} // namespace btx