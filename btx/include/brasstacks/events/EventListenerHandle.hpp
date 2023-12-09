// Credit to kainev for this whole shebang:
// https://codereview.stackexchange.com/questions/252884/

#ifndef BRASSTACKS_EVENTS_EVENTLISTENERHANDLE_HPP
#define BRASSTACKS_EVENTS_EVENTLISTENERHANDLE_HPP

#include "brasstacks/pch.hpp"

namespace btx {

struct EventListenerHandle final {
    uint32_t event_id    = 0u;
    uint32_t listener_id = 0u;
};

} // namespace btx

#endif // BRASSTACKS_EVENTS_EVENTLISTENERHANDLE_HPP