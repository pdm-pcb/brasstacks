#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/Application.hpp"

#include "brasstacks/core/TargetWindow.hpp"
#include "brasstacks/core/Renderer.hpp"
#include "brasstacks/core/state/AppStateMenu.hpp"
#include "brasstacks/core/state/AppStatePlay.hpp"
#include "brasstacks/assets/libraries/MeshLibrary.hpp"

namespace btx {

// =============================================================================
Application::Application(std::string_view const app_name) :
    _current_state             { nullptr },
    _running                   { true },
    _swapchain_destroyed       { false },
    _target_window             { new TargetWindow(app_name) },
    _renderer                  { new Renderer(*this) },
    _renderer_thread           { &Renderer::run, _renderer },
    _simulation                { new Simulation(*this, 120) },
    _simulation_thread         { &Simulation::run, _simulation },
    _state_events              { *this, &Application::_state_transition },
    _window_close_events       { *this, &Application::_window_close },
    _window_size_events        { *this, &Application::_window_size },
    _window_minimize_events    { *this, &Application::_window_minimize },
    _window_restore_events     { *this, &Application::_window_restore },
    _swapchain_recreate_events { *this, &Application::_swapchain_recreate },
    _key_press_events          { *this, &Application::_key_press },
    _mouse_button_press_events { *this, &Application::_mouse_button_press }
{
    _menu_state = new AppStateMenu(*this);
    _play_state = new AppStatePlay(*_target_window);

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

    _renderer->begin_thread();
    _simulation->begin_thread();

    _target_window->show();

    while(_running) {
        TimeKeeper::update_run_times();
        _target_window->poll_events();
        _process_events();

        _current_state->execute();

        std::this_thread::yield();
    }

    _target_window->hide();

    _simulation->end_thread();
    _simulation_thread.join();

    _renderer->end_thread();
    _renderer_thread.join();

    _renderer->wait_device_idle();

    this->shutdown();
}

// =============================================================================
void Application::_process_events() {
    _state_events.process_queue();
    _window_close_events.process_queue();
    _window_size_events.process_queue();
    _window_minimize_events.process_queue();
    _window_restore_events.process_queue();
    _swapchain_recreate_events.process_queue();
    _key_press_events.process_queue();
    _mouse_button_press_events.process_queue();
}

// =============================================================================
void Application::_state_transition(AppStateTransition const &event) {
    if(_current_state != nullptr) {
        _current_state->exit();
    }

    switch(event.next_state_type) {
        case AppState::MENU_STATE: _current_state = _menu_state; break;
        case AppState::PLAY_STATE: _current_state = _play_state; break;
    }

    if(_current_state == nullptr) {
        BTX_CRITICAL("Application transitioned to state that doesn't exist.");
    }

    _current_state->enter();
}

// =============================================================================
void Application::_window_close([[maybe_unused]] WindowCloseEvent const &event)
{
    _running = false;
}

// =============================================================================
void Application::_window_size(WindowSizeEvent const &event) {
    if(!_swapchain_destroyed
       && (event.size.width == 0u || event.size.height == 0u))
    {
        BTX_TRACE("Application received window size {}x{}.",
                  event.size.width,
                  event.size.height);

        _renderer->pause_loop();
        _simulation->pause_loop();

        _renderer->wait_device_idle();

        this->destroy_swapchain_resources();

        _swapchain_destroyed = true;

        return;
    }

    if(_swapchain_destroyed && event.size.width > 0u && event.size.height > 0u)
    {
        BTX_TRACE("Application received window size {}x{}.",
                  event.size.width,
                  event.size.height);

        _renderer->recreate_swapchain();
        this->create_swapchain_resources();

        _renderer->run_loop();
        _simulation->run_loop();

        _swapchain_destroyed = false;
    }
}

// =============================================================================
void Application::_window_minimize(
    [[maybe_unused]] WindowMinimizeEvent const &event)
{
    BTX_TRACE("Application received window minimize.");

    if(!_swapchain_destroyed) {
        _renderer->pause_loop();
        _simulation->pause_loop();

        _renderer->wait_device_idle();

        this->destroy_swapchain_resources();

        _swapchain_destroyed = true;
    }
}

// =============================================================================
void Application::_window_restore(
    [[maybe_unused]] WindowRestoreEvent const &event)
{
    BTX_TRACE("Application received window restore.");

    if(_swapchain_destroyed) {
        _renderer->recreate_swapchain();
        this->create_swapchain_resources();

        _renderer->run_loop();
        _simulation->run_loop();

        _swapchain_destroyed = false;
    }
}

// =============================================================================
void Application::_swapchain_recreate(
    [[maybe_unused]] SwapchainRecreateEvent const &event)
{
    BTX_TRACE("Application received swapchain resize.");

    if(!_swapchain_destroyed) {
        _simulation->pause_loop();

        this->destroy_swapchain_resources();
        _renderer->recreate_swapchain();
        this->create_swapchain_resources();

        _renderer->run_loop();
        _simulation->run_loop();
    }
}

// =============================================================================
void Application::_key_press(KeyPressEvent const &event) {
    if(_current_state == nullptr) {
        BTX_CRITICAL("Application::_key_press before state assigned.");
        return;
    }

    _current_state->key_press(event);
}

// =============================================================================
void Application::_mouse_button_press(MouseButtonPressEvent const &event) {
    if(_current_state == nullptr) {
        BTX_CRITICAL("Application::_mouse_button_press before state assigned.");
        return;
    }

    _current_state->mouse_button_press(event);
}

} // namespace btx