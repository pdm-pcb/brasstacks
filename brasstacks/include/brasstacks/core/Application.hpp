#ifndef BRASSTACKS_CORE_APPLICATION_HPP
#define BRASSTACKS_CORE_APPLICATION_HPP

#include "brasstacks/pch.hpp"

#include "brasstacks/core/state/AppStateMenu.hpp"
#include "brasstacks/core/state/AppStatePlay.hpp"
#include "brasstacks/core/state/AppStatePause.hpp"

#include "brasstacks/events/EventQueue.hpp"
#include "brasstacks/events/state_events.hpp"
#include "brasstacks/events/window_events.hpp"
#include "brasstacks/events/ui_events.hpp"
#include "brasstacks/events/keyboard_events.hpp"
#include "brasstacks/events/mouse_events.hpp"

namespace btx {

class Application {
public:
    explicit Application(std::string_view const app_name);
    virtual ~Application();

    void run();

    virtual void init() = 0;
    virtual void shutdown() = 0;

    virtual void update() = 0;
    virtual void record_commands() const = 0;

    virtual void swapchain_updated() = 0;

    virtual void create_pipeline() = 0;
    virtual void destroy_pipeline() = 0;

    Application() = delete;

    Application(Application &&) = delete;
    Application(Application const &) = delete;

    Application & operator=(Application &&) = delete;
    Application & operator=(Application const &) = delete;

private:
    AppStateBase *_current_state;
    AppStateMenu  _menu_state;
    AppStatePlay  _play_state;
    AppStatePause _pause_state;

    AppState _state_to_resume;

    bool _running;

    EventQueue<AppStateTransition> _state_events;
    EventQueue<WindowEvent>        _window_events;
    EventQueue<UIEvent>          _ui_events;
    EventQueue<KeyboardEvent>      _keyboard_events;
    EventQueue<MouseButtonEvent>   _mouse_button_events;

    void _process_events();
    void _state_transition(AppStateTransition const &event);
    void _window_event(WindowEvent const &event);
    void _ui_event(UIEvent const &event);
    void _keyboard_event(KeyboardEvent const &event);
    void _mouse_button_event(MouseButtonEvent const &event);
};

} // namespace btx

#endif // BRASSTACKS_CORE_APPLICATION_APPLICATION_HPP