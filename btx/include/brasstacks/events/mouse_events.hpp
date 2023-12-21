#ifndef BRASSTACKS_EVENTS_MOUSE_EVENTS_HPP
#define BRASSTACKS_EVENTS_MOUSE_EVENTS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/system/input/btx_keycodes.hpp"

namespace btx {

struct MouseMoveEvent final {
    int32_t x_offset = 0;
    int32_t y_offset = 0;
};

struct MouseButtonPressEvent final {
    Keycode code;
};

struct MouseButtonReleaseEvent final {
    Keycode code;
};

struct MouseScrollEvent final {
    int32_t vert_offset = 0;
    int32_t horiz_offset = 0;
};

} //namespace btx

#endif // BRASSTACKS_EVENTS_MOUSE_EVENTS_HPP