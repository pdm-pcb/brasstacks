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
    _target_window { nullptr },
    _renderer      { nullptr }
{
    // Events can start firing potentially as soon as we let TargetWindow run
    // its message loop
    EventBroker::init();
    // Application uses key releases to controll its running state
    EventBroker::subscribe<KeyReleaseEvent>(this, &Application::on_key_release);
    EventBroker::subscribe<WindowSizeEvent>(this, &Application::on_window_size_event);

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
            vkCmdBuffer const &cmd_buffer = _renderer->begin_recording();

            this->record_commands(cmd_buffer, image_index);

            _renderer->end_recording();
            _renderer->submit_commands();
            _renderer->present_image();

        Timekeeper::frame_end();

        // Check for input and events from the OS
        _target_window->message_loop();
    }

    _renderer->wait_device_idle();

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
void Application::on_window_size_event(WindowSizeEvent const &) {
    if(_renderer == nullptr) {
        return;
    }

    _renderer->wait_device_idle();

    _renderer->destroy_swapchain();
    this->destroy_framebuffers();

    if(RenderConfig::target_window_size.width == 0u
        || RenderConfig::target_window_size.height == 0u)
    {
        return;
    }

    _renderer->create_swapchain();
    this->create_framebuffers(_renderer->device(), _renderer->swapchain());
}

} // namespace btx