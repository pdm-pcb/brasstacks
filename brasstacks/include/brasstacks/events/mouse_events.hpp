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

    int32_t const x_offset;
    int32_t const y_offset;
};

enum class MouseButtonEventType : uint8_t {
    BUTTON_PRESS = 0u,
    BUTTON_RELEASE
};

struct MouseButtonEvent final : public EventBase {
    MouseButtonEvent(MouseButtonEventType const type, Keycode const code) :
        EventBase(),
        event_type { type },
        event_code { code }
    { }

    MouseButtonEvent() = delete;

    MouseButtonEventType event_type;
    Keycode event_code;
};

struct MouseScrollEvent final : public EventBase {
    MouseScrollEvent(int32_t const vert, int32_t const horiz) :
        EventBase(),
        vert_offset { vert },
        horiz_offset { horiz }
    { }

    MouseScrollEvent() = delete;

    int32_t const vert_offset;
    int32_t const horiz_offset;
};

} //namespace btx

#endif // BRASSTACKS_EVENTS_MOUSE_EVENTS_HPP