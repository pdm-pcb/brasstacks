#ifndef BRASSTACKS_CORE_APPLICATION_HPP
#define BRASSTACKS_CORE_APPLICATION_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/events/EventQueue.hpp"
#include "brasstacks/events/state_events.hpp"
#include "brasstacks/events/window_events.hpp"
#include "brasstacks/events/renderer_events.hpp"
#include "brasstacks/events/keyboard_events.hpp"
#include "brasstacks/events/mouse_events.hpp"

namespace btx {

class AppStateBase;
class AppStateMenu;
class AppStatePlay;

class MeshLibrary;

class TargetWindow;
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
    virtual void create_swapchain_resources() = 0;

    inline auto const & target_window() const { return *_target_window; }
    inline auto const & renderer() const { return *_renderer; }

    Application() = delete;

    Application(Application &&) = delete;
    Application(Application const &) = delete;

    Application & operator=(Application &&) = delete;
    Application & operator=(Application const &) = delete;

protected:
    MeshLibrary *_mesh_library;

private:
    AppStateBase *_current_state;
    AppStateMenu *_menu_state;
    AppStatePlay *_play_state;

    bool _running;
    bool _swapchain_destroyed;

    TargetWindow *_target_window;

    Renderer *_renderer;
    std::thread _renderer_thread;

    Simulation *_simulation;
    std::thread _simulation_thread;

    EventQueue<AppStateTransition>     _state_events;
    EventQueue<WindowCloseEvent>       _window_close_events;
    EventQueue<WindowSizeEvent>        _window_size_events;
    EventQueue<WindowMinimizeEvent>    _window_minimize_events;
    EventQueue<WindowRestoreEvent>     _window_restore_events;
    EventQueue<SwapchainRecreateEvent> _swapchain_recreate_events;
    EventQueue<KeyPressEvent>          _key_press_events;
    EventQueue<MouseButtonPressEvent>  _mouse_button_press_events;

    void _process_events();
    void _state_transition(AppStateTransition const &event);
    void _window_close(WindowCloseEvent const &event);
    void _window_size(WindowSizeEvent const &event);
    void _window_minimize(WindowMinimizeEvent const &event);
    void _window_restore(WindowRestoreEvent const &event);
    void _swapchain_recreate(SwapchainRecreateEvent const &event);
    void _key_press(KeyPressEvent const &event);
    void _mouse_button_press(MouseButtonPressEvent const &event);
};

} // namespace btx

#endif // BRASSTACKS_CORE_APPLICATION_APPLICATION_HPP