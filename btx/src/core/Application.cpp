#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/Application.hpp"

#include "brasstacks/core/TargetWindow.hpp"
#include "brasstacks/core/Renderer.hpp"

namespace btx {

// =============================================================================
Application::Application(std::string_view const app_name) :
    _running       { true },
    _target_window { new TargetWindow(app_name) },
    _renderer      { new Renderer(*_target_window) },
    _window_close_events { *this, &Application::on_window_close },
    _key_release_events  { *this, &Application::on_key_release }
{ }

// =============================================================================
Application::~Application() {
    delete _renderer;
    delete _target_window;
}

// =============================================================================
void Application::run() {
    this->init(_renderer->device(), _renderer->swapchain());

    _target_window->show_window();

    uint32_t image_index = std::numeric_limits<uint32_t>::max();
    vkCmdBuffer const *cmd_buffer = nullptr;

    while(_running) {
        _target_window->update();

        _window_close_events.process_queue();
        _key_release_events.process_queue();

        this->update();

        image_index = _renderer->acquire_next_image();
        if(image_index == std::numeric_limits<uint32_t>::max()) {
            continue;
        }

        cmd_buffer = &_renderer->begin_recording();

        this->record_commands(*cmd_buffer, image_index);

        _renderer->end_recording();
        _renderer->submit_commands();

        if(_renderer->present_image() == false) {
            continue;
        }
    }

    _target_window->hide_window();

    _renderer->wait_device_idle();

    this->shutdown();
}

// =============================================================================
void
Application::on_window_close([[maybe_unused]] WindowCloseEvent const &event) {
    _running = false;
}

// =============================================================================
void Application::on_key_release([[maybe_unused]] KeyReleaseEvent const &event)
{
    if(event.code == BTX_KB_ESCAPE) {
        _running = false;
    }
}

} // namespace btx