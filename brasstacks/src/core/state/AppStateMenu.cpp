#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/state/AppStateMenu.hpp"

#include "brasstacks/events/state_events.hpp"

namespace btx {

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