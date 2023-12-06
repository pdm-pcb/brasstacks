#ifndef BRASSTACKS_SYSTEM_APPLICATION_HPP
#define BRASSTACKS_SYSTEM_APPLICATION_HPP

#include "brasstacks/pch.hpp"

namespace btx {

class Application {
public:
    virtual void init()     = 0;
    virtual void shutdown() = 0;

    void run();

    Application();
    virtual ~Application();

    Application(Application &&) = delete;
    Application(const Application &) = delete;

    Application& operator=(Application &&) = delete;
    Application& operator=(const Application &) = delete;

private:
    bool _running;

};

} // namespace btx

#endif // BRASSTACKS_SYSTEM_APPLICATION_HPP