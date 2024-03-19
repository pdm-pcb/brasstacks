#ifndef BRASSTACKS_EVENTS_WINDOW_EVENTS_HPP
#define BRASSTACKS_EVENTS_WINDOW_EVENTS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/events/EventBase.hpp"
#include "brasstacks/config/RenderConfig.hpp"

namespace btx {

// Empty struct to carry the signal
struct WindowCloseEvent final : public EventBase { };

struct WindowSizeEvent final : public EventBase {
    RenderConfig::Size size { };
};

struct WindowMinimizeEvent final : public EventBase { };

struct WindowRestoreEvent final : public EventBase { };

} //namespace btx

#endif // BRASSTACKS_EVENTS_WINDOW_EVENTS_HPP