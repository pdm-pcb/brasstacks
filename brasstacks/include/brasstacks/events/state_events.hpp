#ifndef BRASSTACKS_EVENTS_APP_STATE_EVENTS_HPP
#define BRASSTACKS_EVENTS_APP_STATE_EVENTS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/events/EventBase.hpp"

namespace btx {

enum class AppState : uint8_t {
    MENU_STATE = 0u,
    PLAY_STATE,
    PAUSE_STATE,

    INVALID_STATE = std::numeric_limits<uint8_t>::max()
};

struct AppStateTransition final : public EventBase {
    explicit AppStateTransition(AppState const next_state_type) :
        EventBase(),
        next_state { next_state_type }
    {

    }

    ~AppStateTransition() = default;

    AppStateTransition(AppStateTransition const &rhs) :
        next_state { rhs.next_state }
    { }

    AppStateTransition(AppStateTransition &&rhs) :
        next_state { rhs.next_state }
    {
        rhs.next_state = AppState::INVALID_STATE;
    }

    AppStateTransition & operator=(AppStateTransition const &rhs) {
        next_state = rhs.next_state;
        return *this;
    }

    AppStateTransition & operator=(AppStateTransition &&rhs) {
        next_state = rhs.next_state;
        rhs.next_state = AppState::INVALID_STATE;
        return *this;
    }

    AppStateTransition() = delete;

    AppState next_state;
};

} //namespace btx

#endif // BRASSTACKS_EVENTS_APP_STATE_EVENTS_HPP