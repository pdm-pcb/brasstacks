#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/Application.hpp"

#include "brasstacks/core/TargetWindow.hpp"
#include "brasstacks/core/Renderer.hpp"

namespace btx {

// =============================================================================
Application::Application(std::string_view const app_name)
{
    TargetWindow::init(app_name);
    Renderer::init(Renderer::Config {
        .vsync_on = true
    });
}

// =============================================================================
Application::~Application() {
    Renderer::shutdown();
    TargetWindow::shutdown();
}

// =============================================================================
void Application::run() {
    this->init();
    // TargetWindow::show();

    // while(!TargetWindow::poll_events()) {
        TimeKeeper::update_run_time();
        this->update();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    // }

    // TargetWindow::hide();
    Renderer::wait_device_idle();
    this->shutdown();
}

} // namespace btx