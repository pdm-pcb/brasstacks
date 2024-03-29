#include "brasstacks/brasstacks.hpp"
#include "brasstacks/core/state/AppStatePause.hpp"

namespace btx {

// =============================================================================
AppStatePause::AppStatePause() :
    AppStateBase(AppState::PAUSE_STATE)
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