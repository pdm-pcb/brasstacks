#include "Demo.hpp"
#include "brasstacks/tools/ConsoleLog.hpp"
#include "brasstacks/rendering/Renderer.hpp"

void Demo::init() {
    BTX_TRACE("client app init");
}

void Demo::shutdown() {
    BTX_TRACE("client app shutdown");
}

void Demo::update() {
    // ...
}

Demo::Demo() : btx::Application("Demo") { }