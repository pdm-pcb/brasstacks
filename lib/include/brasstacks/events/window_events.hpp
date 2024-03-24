#ifndef BRASSTACKS_EVENTS_WINDOW_EVENTS_HPP
#define BRASSTACKS_EVENTS_WINDOW_EVENTS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/events/EventBase.hpp"
#include "brasstacks/config/RenderConfig.hpp"

namespace btx {

// Empty struct to carry the signal
struct WindowCloseEvent final : public EventBase {
    WindowCloseEvent() : EventBase() { }
};

struct WindowSizeEvent final : public EventBase {
    explicit WindowSizeEvent(RenderConfig::Size const size) :
        EventBase(),
        size { size }
    { }

    WindowSizeEvent() = delete;

    RenderConfig::Size size;
};

struct WindowMinimizeEvent final : public EventBase {
    WindowMinimizeEvent() : EventBase() { }
};

struct WindowRestoreEvent final : public EventBase {
    WindowRestoreEvent() : EventBase() { }
};

} //namespace btx

#endif // BRASSTACKS_EVENTS_WINDOW_EVENTS_HPP