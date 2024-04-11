#ifndef BRASSTACKS_EVENTS_WINDOW_EVENTS_HPP
#define BRASSTACKS_EVENTS_WINDOW_EVENTS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/events/EventBase.hpp"

namespace btx {

enum class WindowEventType : uint8_t {
    WINDOW_CLOSE = 0u,
    WINDOW_MINIMIZE,
    WINDOW_RESTORE
};

struct WindowEvent final : public EventBase {
    explicit WindowEvent(WindowEventType const event_type) :
        EventBase(),
        type { event_type }
    { }

    WindowEvent() = delete;

    WindowEventType const type;
};

} //namespace btx

#endif // BRASSTACKS_EVENTS_WINDOW_EVENTS_HPP