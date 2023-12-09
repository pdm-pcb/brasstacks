#ifndef BRASSTACKS_EVENTS_MOUSE_EVENTS_HPP
#define BRASSTACKS_EVENTS_MOUSE_EVENTS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/system/input/btx_keycodes.hpp"

namespace btx {

struct MouseMoveEvent {
    int32_t x_offset = 0;
    int32_t y_offset = 0;
};

struct MouseButtonPressEvent {
    Keycode code;
};

struct MouseButtonReleaseEvent {
    Keycode code;
};

struct MouseScrollEvent {
    int32_t vert_offset = 0;
    int32_t horiz_offset = 0;
};

} //namespace btx

#endif // BRASSTACKS_EVENTS_MOUSE_EVENTS_HPP