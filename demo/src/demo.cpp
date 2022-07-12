#include "brasstacks/brasstacks.hpp"
#include "brasstacks/Application/EntryPoint.hpp"

class Demo : public btx::Application {
public:
    Demo() {
        BTX_TRACE("Whee!");
    }

    ~Demo() override {
        BTX_TRACE("Eehw!");
    }
};

btx::Application * btx::create_application() {
    return new Demo();
}