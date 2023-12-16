#ifndef BRASSTACKS_SYSTEM_APPLICATION_HPP
#define BRASSTACKS_SYSTEM_APPLICATION_HPP

#include "brasstacks/pch.hpp"

namespace btx {

struct KeyReleaseEvent;
class TargetWindow;
class Renderer;

class Application {
public:
    void run();

    void on_key_release(KeyReleaseEvent const &event);

    explicit Application(std::string_view const app_name);
    virtual ~Application();

    Application() = delete;

    Application(Application &&) = delete;
    Application(const Application &) = delete;

    Application & operator=(Application &&) = delete;
    Application & operator=(const Application &) = delete;

protected:
    virtual void init()     = 0;
    virtual void shutdown() = 0;
    virtual void update()   = 0;

    void request_draw();

private:
    bool _running;

    TargetWindow *_target_window;
    Renderer     *_renderer;
};

} // namespace btx

#endif // BRASSTACKS_SYSTEM_APPLICATION_HPP