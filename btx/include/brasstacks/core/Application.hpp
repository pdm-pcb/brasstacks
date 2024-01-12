#ifndef BRASSTACKS_CORE_APPLICATION_HPP
#define BRASSTACKS_CORE_APPLICATION_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/events/EventQueue.hpp"
#include "brasstacks/events/window_events.hpp"
#include "brasstacks/events/keyboard_events.hpp"
#include "brasstacks/core/TargetWindow.hpp"

namespace btx {

class Renderer;

class Application {
public:
    explicit Application(std::string_view const app_name);
    virtual ~Application();

    void run();

    virtual void init(Renderer const &renderer) = 0;
    virtual void shutdown() = 0;

    virtual void update() = 0;
    virtual void record_commands() = 0;

    virtual void destroy_swapchain_resources() = 0;
    virtual void recreate_swapchain_resources() = 0;

    void on_window_close(WindowCloseEvent const &event);
    void on_key_release(KeyReleaseEvent const &event);

    Application() = delete;

    Application(Application &&) = delete;
    Application(Application const &) = delete;

    Application & operator=(Application &&) = delete;
    Application & operator=(Application const &) = delete;

private:
    bool _running;
    bool _editor_mode;

    TargetWindow *_target_window;
    std::thread _target_window_thread;

    Renderer *_renderer;

    EventQueue<WindowCloseEvent> _window_close_events;
    EventQueue<KeyReleaseEvent>  _key_release_events;

    void _process_events();
    void _recreate_swapchain();
};

} // namespace btx

#endif // BRASSTACKS_CORE_APPLICATION_HPP