#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/Application.hpp"

#include "brasstacks/core/TargetWindow.hpp"
#include "brasstacks/core/Renderer.hpp"

namespace btx {

// =============================================================================
Application::Application(std::string_view const app_name) :
    _running                   { true },
    // _editor_mode               { true },
    _target_window             { new TargetWindow(app_name) },
    _target_window_thread      { &TargetWindow::run, _target_window },
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

    _target_window->start();
    _renderer->start_thread();
    _simulation->start();

    TimeKeeper::start_app_run_time();

    while(_running) {
        _process_events();
        TimeKeeper::update_app_run_time();

        std::this_thread::yield();
    }

    _simulation->stop();
    _simulation_thread.join();

    _renderer->stop_thread();
    _renderer_thread.join();

    _target_window->stop();
    _target_window_thread.join();

    _renderer->wait_device_idle();

    this->shutdown();
}

// =============================================================================
void Application::recreate_swapchain() {

}

// =============================================================================
void
Application::on_window_close([[maybe_unused]] WindowCloseEvent const &event) {
    _running = false;
}

// =============================================================================
void Application::on_key_press(KeyPressEvent const &event) {
    switch(event.code) {
        case BTX_KB_ESCAPE: _running = false; break;
        case BTX_KB_1: _renderer->toggle_loop(); break;
    }
    if(event.code == BTX_KB_ESCAPE) {
        // if(_editor_mode) {
            _running = false;
        // }
        // else {
        //     _editor_mode = true;
        //     // _target_window->toggle_cursor_capture();
        // }
    }
}

// =============================================================================
void Application::on_mouse_button_press(MouseButtonPressEvent const &event) {
    // if(event.code == BTX_MB_LEFT) {
    //     if(_editor_mode) {
    //         _editor_mode = false;
    //         // _target_window->toggle_cursor_capture();
    //     }
    // }
}

// =============================================================================
void Application::_process_events() {
    _window_close_events.process_queue();
    _key_press_events.process_queue();
    _mouse_button_press_events.process_queue();
}

// =============================================================================
void Application::_recreate_swapchain() {
    this->destroy_swapchain_resources();

    while(_running) {
        auto window_size = _target_window->size();
        if(window_size.width != 0u && window_size.height != 0u) {
            break;
        }
        _process_events();
    }

    if(_running) {
        _renderer->recreate_swapchain();
        this->recreate_swapchain_resources();
    }
}

} // namespace btx