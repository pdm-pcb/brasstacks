#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/Application.hpp"

#include "brasstacks/core/TargetWindow.hpp"
#include "brasstacks/core/Renderer.hpp"

#include <thread>

namespace btx {

// =============================================================================
Application::Application(std::string_view const app_name,
                         uint32_t const app_version)
{
    TargetWindow::init(app_name);
    Renderer::init(Renderer::Config {
            .vsync_on = true
        },
        app_name,
        app_version
    );
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
    //     TimeKeeper::update_run_time();
    //     this->update();
    //     Renderer::run();
    // }

    // TargetWindow::hide();
    Renderer::wait_device_idle();
    this->shutdown();
}

} // namespace btx