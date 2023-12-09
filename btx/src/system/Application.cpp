#include "brasstacks/brasstacks.hpp"
#include "brasstacks/system/Application.hpp"

#include "brasstacks/events/EventBroker.hpp"
#include "brasstacks/system/GraphicsAPI.hpp"
#include "brasstacks/system/window/TargetWindow.hpp"

namespace btx {

// =============================================================================
void Application::run() {
    this->init();

    while(_running) {
        TargetWindow::message_loop();
    }

    this->shutdown();
}

// =============================================================================
void Application::on_window_close(
    [[maybe_unused]] WindowCloseEvent const &event)
{
    BTX_TRACE("Application received WindowClosed");
    _running = false;
}

// =============================================================================
Application::Application(std::string_view const app_name) :
    _running { true },
    _padding { }
{
    ConsoleLog::init();
    GraphicsAPI::init();

    EventBroker::init();
    EventBroker::subscribe<WindowCloseEvent>(
        this,
        &Application::on_window_close
    );

    TargetWindow::init(app_name);
    TargetWindow::create_window();
    TargetWindow::create_surface();
}

Application::~Application() {
    TargetWindow::destroy_surface();
    TargetWindow::shutdown();
    GraphicsAPI::shutdown();
}

} // namespace btx