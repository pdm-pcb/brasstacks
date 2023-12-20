#include "Demo.hpp"
#include "brasstacks/core.hpp"

void Demo::init() {
    BTX_TRACE("client app init");
}

void Demo::shutdown() {
    BTX_TRACE("client app shutdown");
}

void Demo::update() {
    // ...

    request_draw();
}

Demo::Demo() : btx::Application("Demo") { }