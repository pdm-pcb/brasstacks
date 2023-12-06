#include "brasstacks/brasstacks.hpp"
#include "brasstacks/system/Application.hpp"

namespace btx {

// =============================================================================
void Application::run() {
    BTX_INFO("brasstacks v{}", BTX_VER);

    this->init();
    // ...
    this->shutdown();
}

// =============================================================================
Application::Application() :
    _running { true }
{
    ConsoleLog::init();
}

Application::~Application() {
}

} // namespace btx