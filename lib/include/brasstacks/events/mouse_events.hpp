#ifndef BRASSTACKS_EVENTS_MOUSE_EVENTS_HPP
#define BRASSTACKS_EVENTS_MOUSE_EVENTS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/events/EventBase.hpp"
#include "brasstacks/platform/input/btx_keycodes.hpp"

namespace btx {

struct MouseMoveEvent final : public EventBase {
    MouseMoveEvent(int32_t const x, int32_t const y) :
        EventBase(),
        x_offset(x),
        y_offset(y)
    { }

    MouseMoveEvent() = delete;

    int32_t x_offset;
    int32_t y_offset;
};

struct MouseButtonPressEvent final : public EventBase {
    MouseButtonPressEvent() : EventBase() { }
    Keycode code;
};

struct MouseButtonReleaseEvent final : public EventBase {
    MouseButtonReleaseEvent() : EventBase() { }

    Keycode code;
};

struct MouseScrollEvent final : public EventBase {
    MouseScrollEvent(int32_t const vert, int32_t const horiz) :
        EventBase(),
        vert_offset { vert },
        horiz_offset { horiz }
    { }

    MouseScrollEvent() = delete;

    int32_t vert_offset;
    int32_t horiz_offset;
};

} //namespace btx

#endif // BRASSTACKS_EVENTS_MOUSE_EVENTS_HPP