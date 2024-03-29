#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/Application.hpp"

#include "brasstacks/core/TargetWindow.hpp"
#include "brasstacks/core/Renderer.hpp"
#include "brasstacks/core/state/AppStateMenu.hpp"
#include "brasstacks/core/state/AppStatePlay.hpp"
#include "brasstacks/core/state/AppStatePause.hpp"
#include "brasstacks/assets/libraries/MeshLibrary.hpp"

namespace btx {

// =============================================================================
Application::Application(std::string_view const app_name) :
    _current_state             { nullptr },
    _state_to_resume           { AppState::MENU_STATE },
    _running                   { true },
    _target_window             { new TargetWindow(app_name) },
    _renderer                  { new Renderer(*this) },
    _simulation                { new Simulation(*this) },
    _state_events              { *this, &Application::_state_transition },
    _window_events             { *this, &Application::_window_event },
    _keyboard_events           { *this, &Application::_keyboard_event },
    _mouse_button_events       { *this, &Application::_mouse_button_event }
{
    _state_events.subscribe();
    _window_events.subscribe();
    _keyboard_events.subscribe();
    _mouse_button_events.subscribe();

    _menu_state  = new AppStateMenu(*this);
    _play_state  = new AppStatePlay(*this);
    _pause_state = new AppStatePause;

    EventBus::publish(AppStateTransition(AppState::MENU_STATE));

    _mesh_library = new MeshLibrary(*_renderer);
}

// =============================================================================
Application::~Application() {
    delete _menu_state;
    delete _play_state;

    delete _mesh_library;

    delete _simulation;
    delete _renderer;
    delete _target_window;
}

// =============================================================================
void Application::run() {
    this->init(*_renderer);

    _target_window->show();

    while(_running) {
        TimeKeeper::update_run_time();

        _target_window->poll_events();
        _process_events();

        _current_state->execute();
    }

    _target_window->hide();

    _renderer->wait_device_idle();

    this->shutdown();
}

// =============================================================================
void Application::_process_events() {
    _state_events.process_queue();
    _window_events.process_queue();
    _keyboard_events.process_queue();
    _mouse_button_events.process_queue();
}

// =============================================================================
void Application::_state_transition(AppStateTransition const &event) {
    if(_current_state != nullptr) {
        _current_state->exit();
    }

    switch(event.next_state_type) {
        case AppState::MENU_STATE:  _current_state = _menu_state; break;
        case AppState::PLAY_STATE:  _current_state = _play_state; break;
        case AppState::PAUSE_STATE: _current_state = _pause_state; break;
    }

    if(_current_state == nullptr) {
        BTX_CRITICAL("Application transitioned to state that doesn't exist.");
    }

    _current_state->enter();
}

// =============================================================================
void Application::_window_event(WindowEvent const &event) {
    if(event.event_type == WindowEventType::WINDOW_CLOSE) {
        _running = false;
    }
    else if(event.event_type == WindowEventType::WINDOW_MINIMIZE) {
        BTX_TRACE("Application received window minimize.");
        _state_to_resume = _current_state->type();
        _state_events.clear();
        _state_transition(AppStateTransition(AppState::PAUSE_STATE));
    }
    else if(event.event_type == WindowEventType::WINDOW_RESTORE) {
        BTX_TRACE("Application received window restore.");
        _state_events.clear();
        _state_transition(AppStateTransition(_state_to_resume));
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