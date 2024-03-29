#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/state/AppStatePlay.hpp"

namespace btx {

// =============================================================================
AppStatePlay::AppStatePlay(Application &application) :
    AppStateBase(AppState::PLAY_STATE),
    _application { application }
{ }

// =============================================================================
void AppStatePlay::enter() {
    BTX_INFO("Entering AppStatePlay");
    _application.target_window().capture_mouse();
    _application.activate_camera();
}

// =============================================================================
void AppStatePlay::exit() {
    BTX_INFO("Exiting AppStatePlay");
    _application.target_window().release_mouse();
    _application.deactivate_camera();
}

// =============================================================================
void AppStatePlay::execute() {
    _application.update_camera();
    _application.simulation().run();
    _application.renderer().run();
}

// =============================================================================
void AppStatePlay::keyboard_event(KeyboardEvent const &event) {
    switch(event.event_code) {
        case BTX_KB_ESCAPE:
            if(event.event_type == KeyboardEventType::KEY_RELEASE) {
                EventBus::publish(AppStateTransition(AppState::MENU_STATE));
            }
            break;
    }
}

} // namespace btx