#include "brasstacks/core.hpp"
#include "brasstacks/system/Application.hpp"

#include "brasstacks/events/EventBroker.hpp"
#include "brasstacks/events/keyboard_events.hpp"

#include "brasstacks/system/TargetWindow.hpp"
#include "brasstacks/rendering/Renderer.hpp"

namespace btx {

// =============================================================================
Application::Application(std::string_view const app_name) :
    _running       { true },
    _paused        { false },
    _target_window { nullptr },
    _renderer      { nullptr }
{
    // Events can start firing potentially as soon as we let TargetWindow run
    // its message loop
    EventBroker::init();

    EventBroker::subscribe<WindowCloseEvent>(this, &Application::on_window_close);
    EventBroker::subscribe<WindowSizeEvent>(this, &Application::on_window_size_event);
    EventBroker::subscribe<WindowMinimizeEvent>(this, &Application::on_window_minimize);
    EventBroker::subscribe<WindowRestoreEvent>(this, &Application::on_window_restore);
    EventBroker::subscribe<KeyReleaseEvent>(this, &Application::on_key_release);

    // Instantiate the platform window and renderer backend
    _target_window = TargetWindow::create(app_name);
    _renderer = new Renderer(*_target_window);
}

Application::~Application() {
    delete _renderer;
    delete _target_window;
}

// =============================================================================
void Application::run() {
    // Give the user a chance to bring up their data
    this->init(_renderer->device(), _renderer->swapchain());

    _target_window->show_window();

    while(_running) {
        // Tick the clock
        Timekeeper::update();

        // Have the client do per-frame processing and submit draw calls
        this->update();

        Timekeeper::frame_start();

            // Let the renderer go to town on a frame
            uint32_t const image_index = _renderer->acquire_next_image();

            if(image_index == std::numeric_limits<uint32_t>::max()) {
                _destroy_and_recreate_swapchain();
                continue;
            }

            vkCmdBuffer const &cmd_buffer = _renderer->begin_recording();

            this->record_commands(cmd_buffer, image_index);

            _renderer->end_recording();
            _renderer->submit_commands();

            if(_renderer->present_image() == false) {
                _destroy_and_recreate_swapchain();
            }

        Timekeeper::frame_end();

        do {
            // Check for input and events from the OS
            _target_window->message_loop();
        } while(_running && _paused);
    }

    // Kill Events first so nothing unexpected pops up during destruction
    EventBroker::shutdown();

    _renderer->wait_device_idle();

    // Give the user a chance to clean up
    this->shutdown();
}

// =============================================================================
void Application::on_window_close(WindowCloseEvent const &) {
    _running = false;
}

// =============================================================================
void Application::on_window_size_event(WindowSizeEvent const &) {
    if(_renderer == nullptr) {
        return;
    }
    _destroy_and_recreate_swapchain();
}

// =============================================================================
void Application::on_window_minimize(WindowMinimizeEvent const &) {
    if(_renderer == nullptr) {
        return;
    }

    _renderer->wait_device_idle();

    _renderer->destroy_swapchain_and_resources();
    this->destroy_framebuffers();

    _paused = true;
}

// =============================================================================
void Application::on_window_restore(WindowRestoreEvent const &) {
    if(_renderer == nullptr) {
        return;
    }

    _renderer->create_swapchain_and_resources();
    this->create_framebuffers(_renderer->device(), _renderer->swapchain());

    _paused = false;
}

// =============================================================================
void Application::on_key_release(KeyReleaseEvent const &event) {
    if(event.code == BTX_KB_ESCAPE) {
        _running = false;
    }
}

// =============================================================================
void Application::_destroy_and_recreate_swapchain() {
    _renderer->wait_device_idle();

    _renderer->destroy_swapchain_and_resources();
    this->destroy_framebuffers();

    _renderer->create_swapchain_and_resources();
    this->create_framebuffers(_renderer->device(), _renderer->swapchain());
}

} // namespace btx