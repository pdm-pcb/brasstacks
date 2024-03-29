#ifndef BRASSTACKS_EVENTS_APP_STATE_EVENTS_HPP
#define BRASSTACKS_EVENTS_APP_STATE_EVENTS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/events/EventBase.hpp"

namespace btx {

enum class AppState : uint8_t {
    MENU_STATE = 0u,
    PLAY_STATE,
    PAUSE_STATE
};

struct AppStateTransition final : public EventBase {
    explicit AppStateTransition(AppState const next_state) :
        EventBase(),
        next_state_type { next_state }
    { }

    AppStateTransition() = delete;

    AppState next_state_type;
};

} //namespace btx

#endif // BRASSTACKS_EVENTS_APP_STATE_EVENTS_HPP