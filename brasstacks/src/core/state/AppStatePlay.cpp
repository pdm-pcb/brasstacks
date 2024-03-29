#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/state/AppStatePlay.hpp"

namespace btx {

// =============================================================================
AppStatePlay::AppStatePlay(Application &application) :
    _application { application }
{ }

// =============================================================================
void AppStatePlay::enter() {
    BTX_INFO("Entering AppStatePlay");
    _application.target_window().capture_mouse();
}

// =============================================================================
void AppStatePlay::exit() {
    BTX_INFO("Exiting AppStatePlay");
    _application.target_window().release_mouse();
}

// =============================================================================
void AppStatePlay::execute() {
}

// =============================================================================
void AppStatePlay::key_press(KeyPressEvent const &event) {
    switch(event.code) {
        case BTX_KB_ESCAPE:
            EventBus::publish(AppStateTransition(AppState::MENU_STATE));
            break;
    }
}

} // namespace btx