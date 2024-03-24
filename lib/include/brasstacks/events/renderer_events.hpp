#ifndef BRASSTACKS_EVENTS_RENDERER_EVENTS_HPP
#define BRASSTACKS_EVENTS_RENDERER_EVENTS_HPP

#include "brasstacks/pch.hpp"
#include "brasstacks/events/EventBase.hpp"

namespace btx {

struct SwapchainRecreateEvent : public EventBase {
    SwapchainRecreateEvent() : EventBase() { }
};

} // namespace btx

#endif // BRASSTACKS_EVENTS_RENDERER_EVENTS_HPP