#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/state/AppStatePlay.hpp"

#include "brasstacks/tools/cameras/CameraController.hpp"

namespace btx {

// =============================================================================
void AppStatePlay::enter() {
    BTX_INFO("Entering AppStatePlay");
    TargetWindow::capture_mouse();
    CameraController::activate_camera();
}

// =============================================================================
void AppStatePlay::exit() {
    BTX_INFO("Exiting AppStatePlay");
    TargetWindow::release_mouse();
    CameraController::deactivate_camera();
}

// =============================================================================
void AppStatePlay::execute() {
    CameraController::update_camera();
    Simulation::run();
    Renderer::run();
}

// =============================================================================
void AppStatePlay::keyboard_event(KeyboardEvent const &event) {
    switch(event.code) {
        case BTX_KB_ESCAPE:
            if(event.type == KeyboardEventType::KEY_RELEASE) {
                EventBus::publish(AppStateTransition(AppState::MENU_STATE));
            }
            break;
    }
}

} // namespace btx