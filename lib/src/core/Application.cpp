#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/Application.hpp"

#include "brasstacks/core/TargetWindow.hpp"
#include "brasstacks/core/Renderer.hpp"

namespace btx {

// =============================================================================
Application::Application(std::string_view const app_name) :
    _running                 { true },
    _swapchain_destroyed     { false },
    _target_window           { new TargetWindow(app_name) },
    _renderer                { new Renderer(*this) },
    _renderer_thread         { &Renderer::run, _renderer },
    _simulation              { new Simulation(*this, 120) },
    _simulation_thread       { &Simulation::run, _simulation },
    _window_close_events     { *this, &Application::_on_window_close },
    _window_size_events      { *this, &Application::_on_window_size },
    _window_minimize_events  { *this, &Application::_on_window_minimize },
    _window_restore_events   { *this, &Application::_on_window_restore },
    _swapchain_resize_events { *this, &Application::_on_swapchain_recreate },
    _key_press_events        { *this, &Application::_on_key_press }
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

    _renderer->begin_thread();
    _simulation->begin_thread();
    _target_window->show();

    while(_running) {
        TimeKeeper::update_run_times();
        _target_window->poll_events();
        _process_events();

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
    _window_close_events.process_queue();
    _window_size_events.process_queue();
    _window_minimize_events.process_queue();
    _window_restore_events.process_queue();
    _swapchain_resize_events.process_queue();
    _key_press_events.process_queue();
}

// =============================================================================
void Application::_on_window_close(
    [[maybe_unused]] WindowCloseEvent const &event)
{
    _running = false;
}

// =============================================================================
void Application::_on_window_size(WindowSizeEvent const &event) {

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
        this->recreate_swapchain_resources();

        _renderer->run_loop();
        _simulation->run_loop();

        _swapchain_destroyed = false;
    }
}

// =============================================================================
void Application::_on_window_minimize(
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
void Application::_on_window_restore(
    [[maybe_unused]] WindowRestoreEvent const &event)
{
    BTX_TRACE("Application received window restore.");

    if(_swapchain_destroyed) {
        _renderer->recreate_swapchain();
        this->recreate_swapchain_resources();

        _renderer->run_loop();
        _simulation->run_loop();

        _swapchain_destroyed = false;
    }
}

// =============================================================================
void Application::_on_swapchain_recreate(
    [[maybe_unused]] SwapchainRecreateEvent const &event)
{
    BTX_TRACE("Application received swapchain resize.");

    if(!_swapchain_destroyed) {
        _simulation->pause_loop();

        this->destroy_swapchain_resources();
        _renderer->recreate_swapchain();
        this->recreate_swapchain_resources();

        _renderer->run_loop();
        _simulation->run_loop();
    }
}

// =============================================================================
void Application::_on_key_press(KeyPressEvent const &event) {
    switch(event.code) {
        case BTX_KB_ESCAPE:
            _running = false;
            break;
    }
}

} // namespace btx