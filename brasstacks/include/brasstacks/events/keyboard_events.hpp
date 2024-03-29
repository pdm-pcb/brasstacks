#ifndef BRASSTACKS_EVENTS_KEYBOARD_EVENTS_HPP
#define BRASSTACKS_EVENTS_KEYBOARD_EVENTS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/events/EventBase.hpp"
#include "brasstacks/platform/input/btx_keycodes.hpp"

namespace btx {

enum class KeyboardEventType : uint8_t {
    KEY_PRESS = 0u,
    KEY_RELEASE
};

struct KeyboardEvent final : public EventBase {
    KeyboardEvent(KeyboardEventType const type, Keycode const code) :
        EventBase(),
        event_type { type },
        event_code { code }
    { }

    KeyboardEvent() = delete;

    KeyboardEventType const event_type;
    Keycode const event_code;
};

} //namespace btx

#endif // BRASSTACKS_EVENTS_KEYBOARD_EVENTS_HPP