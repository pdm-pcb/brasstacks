#ifndef DEMO_HPP
#define DEMO_HPP

#include "brasstacks/system/Application.hpp"

namespace btx {
    class Renderer;
}

class Demo final : public btx::Application {
public:
    void init(btx::Renderer &renderer) override;
    void shutdown() override;
    void update() override;

    Demo();
    ~Demo() override = default;
};

#endif // DEMO_HPP