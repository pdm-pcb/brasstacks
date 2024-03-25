#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/state/AppStateMenu.hpp"

#include "brasstacks/events/state_events.hpp"

namespace btx {

// =============================================================================
AppStateMenu::AppStateMenu(Application &application) :
    _application { application }
{ }

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
}

// =============================================================================
void AppStateMenu::key_press(KeyPressEvent const &event) {
    switch(event.code) {
        case BTX_KB_ESCAPE:
            EventBus::publish(WindowCloseEvent { });
            break;
    }
}

// =============================================================================
void AppStateMenu::mouse_button_press(MouseButtonPressEvent const &event) {
    switch(event.code) {
        case BTX_MB_LEFT:
            EventBus::publish(AppStateTransition(AppState::PLAY_STATE));
            break;
    }
}

} // namespace btx