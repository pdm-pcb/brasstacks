#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/Application.hpp"

#include "brasstacks/core/TargetWindow.hpp"
#include "brasstacks/core/Renderer.hpp"

namespace btx {

// =============================================================================
Application::Application(std::string_view const app_name) :
    _running                   { true },
    _target_window             { new TargetWindow(app_name) },
    _renderer                  { new Renderer(*this) },
    _renderer_thread           { &Renderer::run, _renderer },
    _simulation                { new Simulation(*this, 120) },
    _simulation_thread         { &Simulation::run, _simulation },
    _window_close_events       { *this, &Application::on_window_close },
    _key_press_events          { *this, &Application::on_key_press },
    _mouse_button_press_events { *this, &Application::on_mouse_button_press }
{ }

// =============================================================================
Application::~Application() {
    delete _simulation;
    delete _renderer;
    delete _target_window;
}

// =============================================================================
void Application::run() {
    this->init(*_renderer);

    _renderer->start_thread();
    _simulation->start_thread();
    _target_window->show();

    while(_running) {
        TimeKeeper::update_run_times();
        _target_window->poll_events();
        _process_events();

        std::this_thread::yield();
    }

    _target_window->hide();

    _simulation->stop_thread();
    _simulation_thread.join();

    _renderer->stop_thread();
    _renderer_thread.join();

    _renderer->wait_device_idle();

    this->shutdown();
}

// =============================================================================
void
Application::on_window_close([[maybe_unused]] WindowCloseEvent const &event) {
    _running = false;
}

// =============================================================================
void Application::on_key_press(KeyPressEvent const &event) {
    switch(event.code) {
        case BTX_KB_ESCAPE:
            _running = false;
            break;

        case BTX_KB_PERIOD:
            _simulation->toggle_loop();
            break;

        case BTX_KB_FRONT_SLASH:
            _renderer->toggle_loop();
            break;
    }
}

// =============================================================================
void Application::on_mouse_button_press(MouseButtonPressEvent const &event) {
}

// =============================================================================
void Application::_process_events() {
    _window_close_events.process_queue();
    _key_press_events.process_queue();
    _mouse_button_press_events.process_queue();
}

} // namespace btx