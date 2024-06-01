#ifndef DEMO_HPP
#define DEMO_HPP

#include "brasstacks/brasstacks.hpp"

class Demo final : public btx::Application {
public:
    Demo();
    ~Demo() override = default;

    void init() override;
    void shutdown() override;

    void update() override;
    void draw() override;
};

#endif // DEMO_HPP