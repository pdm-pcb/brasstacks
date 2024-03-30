#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/state/AppStateMenu.hpp"

#include "brasstacks/events/state_events.hpp"

namespace btx {

// =============================================================================
AppStateMenu::AppStateMenu(Application &application) :
    AppStateBase(AppState::MENU_STATE),
    _application { application }
{ }

// =============================================================================
void AppStateMenu::execute() {
    _application.simulation().run();
    Renderer::run();
}

// =============================================================================
void AppStateMenu::keyboard_event(KeyboardEvent const &event) {
    switch(event.event_code) {
        case BTX_KB_ESCAPE:
            if(event.event_type == KeyboardEventType::KEY_RELEASE) {
                EventBus::publish(WindowEvent(WindowEventType::WINDOW_CLOSE));
            }
            break;
    }
}

// =============================================================================
void AppStateMenu::mouse_button_event(MouseButtonEvent const &event) {
    switch(event.event_code) {
        case BTX_MB_LEFT:
            if(event.event_type == MouseButtonEventType::BUTTON_RELEASE) {
                EventBus::publish(AppStateTransition(AppState::PLAY_STATE));
            }
            break;
    }
}

} // namespace btx