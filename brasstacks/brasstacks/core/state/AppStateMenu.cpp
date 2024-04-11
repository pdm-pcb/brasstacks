#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/state/AppStateMenu.hpp"

#include "brasstacks/events/state_events.hpp"

namespace btx {

// =============================================================================
void AppStateMenu::enter() {
    BTX_INFO("Entering AppStateMenu");
}

// =============================================================================
void AppStateMenu::exit() {
    BTX_INFO("Exiting AppStateMenu");
}

// =============================================================================
void AppStateMenu::execute() {
    Simulation::run();
    Renderer::run();
}

// =============================================================================
void AppStateMenu::keyboard_event(KeyboardEvent const &event) {
    switch(event.code) {
        case BTX_KB_ESCAPE:
            if(event.type == KeyboardEventType::KEY_RELEASE) {
                BTX_TRACE("AppStateMenu publishing window close event");
                EventBus::publish(WindowEvent(WindowEventType::WINDOW_CLOSE));
            }
            break;
    }
}

// =============================================================================
void AppStateMenu::mouse_button_event(MouseButtonEvent const &event) {
    switch(event.code) {
        case BTX_MB_LEFT:
            if(event.type == MouseButtonEventType::BUTTON_RELEASE) {
                EventBus::publish(AppStateTransition(AppState::PLAY_STATE));
            }
            break;
    }
}

} // namespace btx