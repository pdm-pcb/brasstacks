#ifndef BRASSTACKS_EVENTS_MOUSE_EVENTS_HPP
#define BRASSTACKS_EVENTS_MOUSE_EVENTS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/events/EventBase.hpp"
#include "brasstacks/platform/input/btx_keycodes.hpp"

namespace btx {

struct MouseMoveEvent final : public EventBase {
    MouseMoveEvent(int32_t const x_offset, int32_t const y_offset) :
        EventBase(),
        x(x_offset),
        y(y_offset)
    { }

    MouseMoveEvent() = delete;

    int32_t const x;
    int32_t const y;
};

enum class MouseButtonEventType : uint8_t {
    BUTTON_PRESS = 0u,
    BUTTON_RELEASE
};

struct MouseButtonEvent final : public EventBase {
    MouseButtonEvent(MouseButtonEventType const event_type,
                     Keycode const event_code) :
        EventBase(),
        type { event_type },
        code { event_code }
    { }

    MouseButtonEvent() = delete;

    MouseButtonEventType type;
    Keycode code;
};

struct MouseScrollEvent final : public EventBase {
    MouseScrollEvent(int32_t const vert_offset, int32_t const horiz_offset) :
        EventBase(),
        vert { vert_offset },
        horiz { horiz_offset }
    { }

    MouseScrollEvent() = delete;

    int32_t const vert;
    int32_t const horiz;
};

} //namespace btx

#endif // BRASSTACKS_EVENTS_MOUSE_EVENTS_HPP