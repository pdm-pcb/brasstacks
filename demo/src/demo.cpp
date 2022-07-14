#include "brasstacks/brasstacks.hpp"
#include "brasstacks/Application/EntryPoint.hpp"

class Demo : public btx::Application {
public:
    Demo() {
        BTX_TRACE("User constructor");
    }

    ~Demo() override {
        BTX_TRACE("User destructor");
    }
};

btx::Application * btx::create_application() {
    return new Demo();
}