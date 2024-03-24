#ifndef BRASSTACKS_EVENTS_EVENTBASE_HPP
#define BRASSTACKS_EVENTS_EVENTBASE_HPP

#include "brasstacks/pch.hpp"

namespace btx {

struct EventBase {

protected:
    EventBase() = default;
    ~EventBase() = default;

    EventBase(EventBase &&) = default;
    EventBase(EventBase const &) = default;

    EventBase & operator=(EventBase &&) = default;
    EventBase & operator=(EventBase const &) = default;
};

} // namespace btx


#endif // BRASSTACKS_EVENTS_EVENTBASE_HPP