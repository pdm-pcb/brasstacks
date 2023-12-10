#include "brasstacks/brasstacks.hpp"
#include "brasstacks/system/Application.hpp"

#include "brasstacks/events/EventBroker.hpp"
#include "brasstacks/system/GraphicsAPI.hpp"
#include "brasstacks/system/window/TargetWindow.hpp"

namespace btx {

// =============================================================================
void Application::run() {
    TargetWindow::show_window();

    this->init();

    while(_running) {
        TargetWindow::message_loop();
    }

    this->shutdown();
}

// =============================================================================
void Application::on_key_release(KeyReleaseEvent const &event) {
    if(event.code == BTX_KB_ESCAPE) {
        _running = false;
    }
}

// =============================================================================
Application::Application(std::string_view const app_name) :
    _running { true },
    _padding { }
{
    ConsoleLog::init();
    EventBroker::init();
    GraphicsAPI::init();

    EventBroker::subscribe<KeyReleaseEvent>(
        this,
        &Application::on_key_release
    );

    TargetWindow::init(app_name);
    TargetWindow::create_window();
    TargetWindow::create_surface();
}

Application::~Application() {
    TargetWindow::destroy_surface();
    TargetWindow::shutdown();
    GraphicsAPI::shutdown();
    EventBroker::shutdown();
}

} // namespace btx