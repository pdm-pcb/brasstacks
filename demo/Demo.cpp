#include "Demo.hpp"
#include "brasstacks/tools/ConsoleLog.hpp"
// #include "brasstacks/rendering/Renderer.hpp"

void Demo::init(btx::Renderer &renderer) {
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