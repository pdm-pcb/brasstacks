#ifndef BRASSTACKS_EVENTS_KEYBOARD_EVENTS_HPP
#define BRASSTACKS_EVENTS_KEYBOARD_EVENTS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/events/EventBase.hpp"
#include "brasstacks/platform/input/btx_keycodes.hpp"

namespace btx {

struct KeyPressEvent final : public EventBase {
    explicit KeyPressEvent(Keycode const key) :
        EventBase(),
        code { key }
    { }

    KeyPressEvent() = delete;

    Keycode code;
};

struct KeyReleaseEvent final : public EventBase  {
    explicit KeyReleaseEvent(Keycode const key) :
        EventBase(),
        code { key }
    { }

    KeyReleaseEvent() = delete;

    Keycode code;
};

} //namespace btx

#endif // BRASSTACKS_EVENTS_KEYBOARD_EVENTS_HPP