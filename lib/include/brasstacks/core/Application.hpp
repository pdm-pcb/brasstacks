#ifndef BRASSTACKS_CORE_APPLICATION_HPP
#define BRASSTACKS_CORE_APPLICATION_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/events/EventQueue.hpp"
#include "brasstacks/events/window_events.hpp"
#include "brasstacks/events/keyboard_events.hpp"
#include "brasstacks/events/renderer_events.hpp"
#include "brasstacks/core/TargetWindow.hpp"

namespace btx {

class Renderer;
class Simulation;

class Application {
public:
    explicit Application(std::string_view const app_name);
    virtual ~Application();

    void run();

    virtual void init(Renderer const &renderer) = 0;
    virtual void shutdown() = 0;

    virtual void update() = 0;
    virtual void record_commands() const = 0;

    virtual void destroy_swapchain_resources() = 0;
    virtual void recreate_swapchain_resources() = 0;

    inline auto const & target_window() const { return *_target_window; }

    Application() = delete;

    Application(Application &&) = delete;
    Application(Application const &) = delete;

    Application & operator=(Application &&) = delete;
    Application & operator=(Application const &) = delete;

private:
    bool _running;
    bool _swapchain_destroyed;

    TargetWindow *_target_window;

    Renderer *_renderer;
    std::thread _renderer_thread;

    Simulation *_simulation;
    std::thread _simulation_thread;

    EventQueue<WindowCloseEvent>     _window_close_events;
    EventQueue<WindowSizeEvent>      _window_size_events;
    EventQueue<WindowMinimizeEvent>  _window_minimize_events;
    EventQueue<WindowRestoreEvent>   _window_restore_events;
    EventQueue<SwapchainRecreateEvent> _swapchain_resize_events;
    EventQueue<KeyPressEvent>        _key_press_events;

    void _process_events();
    void _on_window_close(WindowCloseEvent const &event);
    void _on_window_size(WindowSizeEvent const &event);
    void _on_window_minimize(WindowMinimizeEvent const &event);
    void _on_window_restore(WindowRestoreEvent const &event);
    void _on_swapchain_recreate(SwapchainRecreateEvent const &event);
    void _on_key_press(KeyPressEvent const &event);
};

} // namespace btx

#endif // BRASSTACKS_CORE_APPLICATION_HPP