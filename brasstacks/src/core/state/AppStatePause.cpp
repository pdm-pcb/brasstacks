#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/state/AppStatePause.hpp"

namespace btx {

// =============================================================================
AppStatePause::AppStatePause(Application &application) :
    AppStateBase(AppState::PAUSE_STATE),
    _application { application }
{ }

// =============================================================================
void AppStatePause::enter() {
    BTX_INFO("Entering AppStatePause");
}

// =============================================================================
void AppStatePause::exit() {
    BTX_INFO("Exiting AppStatePause");
}

} // namespace btx