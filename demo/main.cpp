#include "brasstacks/brasstacks.hpp"

class App final : public btx::Application {
public:
    void init() override {
        CONSOLE_TRACE("client app init");
    }

    void shutdown() override {
        CONSOLE_TRACE("client app shutdown");
    }

    App() : btx::Application("Demo") { }
};

int main() {
    auto app = std::make_unique<App>();
    app->run();
    return 0;
}