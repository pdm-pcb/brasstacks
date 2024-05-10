#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/Application.hpp"

#include "brasstacks/core/TargetWindow.hpp"
#include "brasstacks/core/Simulation.hpp"
#include "brasstacks/core/Renderer.hpp"

#include "brasstacks/assets/libraries/TextureLibrary.hpp"

namespace btx {

// =============================================================================
Application::Application(std::string_view const app_name) :
    _current_state       { nullptr },
    _menu_state          { },
    _play_state          { },
    _pause_state         { },
    _state_to_resume     { AppState::INVALID_STATE },
    _running             { true },
    _state_events        { *this, &Application::_state_transition },
    _window_events       { *this, &Application::_window_event },
    _ui_events           { *this, &Application::_ui_event },
    _keyboard_events     { *this, &Application::_keyboard_event },
    _mouse_button_events { *this, &Application::_mouse_button_event }
{
    _state_transition(AppStateTransition(AppState::MENU_STATE));

    _state_events.subscribe();
    _window_events.subscribe();
    _ui_events.subscribe();
    _keyboard_events.subscribe();
    _mouse_button_events.subscribe();

    TargetWindow::init(app_name);
    Simulation::init(this);
    Renderer::init(this);
}

// =============================================================================
Application::~Application() {
    Renderer::shutdown();
    Simulation::shutdown();
    TargetWindow::shutdown();
}

// =============================================================================
void Application::run() {
    this->init();

    TargetWindow::show();

    while(_running) {
        TimeKeeper::update_run_time();

        TargetWindow::poll_events();
        _process_events();

        _current_state->execute();
    }

    TargetWindow::hide();

    Renderer::wait_device_idle();

    this->shutdown();
}

// =============================================================================
void Application::_process_events() {
    _state_events.process_queue();
    _window_events.process_queue();
    _ui_events.process_queue();
    _keyboard_events.process_queue();
    _mouse_button_events.process_queue();
}

// =============================================================================
void Application::_state_transition(AppStateTransition const &event) {
    if(_current_state != nullptr) {
        _current_state->exit();
    }

    switch(event.next_state) {
        case AppState::MENU_STATE:  _current_state = &_menu_state;  break;
        case AppState::PLAY_STATE:  _current_state = &_play_state;  break;
        case AppState::PAUSE_STATE: _current_state = &_pause_state; break;
        case AppState::INVALID_STATE:
            BTX_CRITICAL("Application received invalid state transition.");
            return;
    }

    if(_current_state == nullptr) {
        BTX_CRITICAL("Application transitioned to state that doesn't exist.");
        return;
    }

    _current_state->enter();
}

// =============================================================================
void Application::_window_event(WindowEvent const &event) {
    if(event.type == WindowEventType::WINDOW_CLOSE) {
        BTX_TRACE("Application received window close.");
        _running = false;
    }
    else if(event.type == WindowEventType::WINDOW_MINIMIZE) {
        BTX_TRACE("Application received window minimize.");
        _state_to_resume = _current_state->type();
        _state_events.clear();
        _state_transition(AppStateTransition(AppState::PAUSE_STATE));
    }
    else if(event.type == WindowEventType::WINDOW_RESTORE) {
        BTX_TRACE("Application received window restore.");
        _state_events.clear();
        _state_transition(AppStateTransition(_state_to_resume));
    }
}

// =============================================================================
void Application::_ui_event(UIEvent const &event) {
    if(event.type == UIEventType::UI_EXIT) {
        BTX_TRACE("Application received UI exit.");
        _running = false;
    }
    else if(event.type == UIEventType::UI_CHANGE_DEVICE) {
        BTX_TRACE("Application received UI change device.");
        Renderer::change_device();
    }
    else if(event.type == UIEventType::UI_WINDOW_RESIZE) {
        BTX_TRACE("Application received UI window resize.");
        TargetWindow::size_and_place();
    }
    else if(event.type == UIEventType::UI_CHANGE_ANISO) {
        BTX_TRACE("Application received UI change aniso.");
        TextureLibrary::update_samplers();
    }
    else if(event.type == UIEventType::UI_CHANGE_MSAA) {
        BTX_TRACE("Application received UI change MSAA.");
        // Renderer::recreate_render_pass();
    }
    else if(event.type == UIEventType::UI_TOGGLE_VSYNC) {
        BTX_TRACE("Application received UI toggle VSync.");
        Renderer::recreate_swapchain();
    }
}

// =============================================================================
void Application::_keyboard_event(KeyboardEvent const &event) {
    if(_current_state == nullptr) {
        BTX_CRITICAL("Keyboard event before Application state set.");
    }

    _current_state->keyboard_event(event);
}

// =============================================================================
void Application::_mouse_button_event(MouseButtonEvent const &event) {
    if(_current_state == nullptr) {
        BTX_CRITICAL("Mouse button event before Application state set.");
    }

    _current_state->mouse_button_event(event);
}

} // namespace btx