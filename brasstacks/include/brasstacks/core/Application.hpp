#ifndef BRASSTACKS_CORE_APPLICATION_HPP
#define BRASSTACKS_CORE_APPLICATION_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/events/EventQueue.hpp"
#include "brasstacks/events/state_events.hpp"
#include "brasstacks/events/window_events.hpp"
#include "brasstacks/events/keyboard_events.hpp"
#include "brasstacks/events/mouse_events.hpp"

namespace btx {

class AppStateBase;
class AppStateMenu;
class AppStatePlay;
class AppStatePause;

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

    virtual void activate_camera() = 0;
    virtual void deactivate_camera() = 0;
    virtual void update_camera() = 0;

    inline auto const & target_window() const { return *_target_window; }
    inline auto & renderer() const { return *_renderer; }
    inline auto & simulation() const { return *_simulation; }

    Application() = delete;

    Application(Application &&) = delete;
    Application(Application const &) = delete;

    Application & operator=(Application &&) = delete;
    Application & operator=(Application const &) = delete;

protected:
    MeshLibrary *_mesh_library;

private:
    AppStateBase  *_current_state;
    AppStateMenu  *_menu_state;
    AppStatePlay  *_play_state;
    AppStatePause *_pause_state;

    AppState _state_to_resume;

    bool _running;

    TargetWindow *_target_window;
    Renderer *_renderer;
    Simulation *_simulation;

    EventQueue<AppStateTransition> _state_events;
    EventQueue<WindowEvent>        _window_events;
    EventQueue<KeyboardEvent>      _keyboard_events;
    EventQueue<MouseButtonEvent>   _mouse_button_events;

    void _process_events();
    void _state_transition(AppStateTransition const &event);
    void _window_event(WindowEvent const &event);
    void _keyboard_event(KeyboardEvent const &event);
    void _mouse_button_event(MouseButtonEvent const &event);
};

} // namespace btx

#endif // BRASSTACKS_CORE_APPLICATION_APPLICATION_HPP