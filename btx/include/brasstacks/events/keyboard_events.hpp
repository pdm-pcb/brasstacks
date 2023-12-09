#ifndef BRASSTACKS_EVENTS_KEYBOARD_EVENTS_HPP
#define BRASSTACKS_EVENTS_KEYBOARD_EVENTS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/system/input/btx_keycodes.hpp"

namespace btx {

struct KeyPressEvent {
    Keycode code;
};

struct KeyReleaseEvent {
    Keycode code;
};

} //namespace btx

#endif // BRASSTACKS_EVENTS_KEYBOARD_EVENTS_HPP