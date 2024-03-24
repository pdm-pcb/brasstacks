#ifndef BRASSTACKS_EVENTS_KEYBOARD_EVENTS_HPP
#define BRASSTACKS_EVENTS_KEYBOARD_EVENTS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/events/EventBase.hpp"
#include "brasstacks/platform/input/btx_keycodes.hpp"

namespace btx {

struct KeyPressEvent final : public EventBase {
    explicit KeyPressEvent(Keycode const code) :
        EventBase(),
        code { code }
    { }

    KeyPressEvent() = delete;

    Keycode code;
};

struct KeyReleaseEvent final : public EventBase  {
    explicit KeyReleaseEvent(Keycode const code) :
        EventBase(),
        code { code }
    { }

    KeyReleaseEvent() = delete;

    Keycode code;
};

} //namespace btx

#endif // BRASSTACKS_EVENTS_KEYBOARD_EVENTS_HPP