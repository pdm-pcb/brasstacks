#include "brasstacks/brasstacks.hpp"

class App final : public btx::Application {
public:
    void init() override {
        TRACE("client app init");
    }

    void shutdown() override {
        TRACE("client app shutdown");
    }
};

int main() {
    auto app = std::make_unique<App>();
    app->run();
    return 0;
}