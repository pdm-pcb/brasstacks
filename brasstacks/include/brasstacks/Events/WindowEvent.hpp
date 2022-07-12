#ifndef BRASSTACKS_EVENTS_WINDOWEVENT_HPP
#define BRASSTACKS_EVENTS_WINDOWEVENT_HPP

#include "brasstacks/Events/Event.hpp"

namespace btx {

class WindowCloseEvent : public Event {
public:
	WindowCloseEvent();
};

} // namespace btx

#endif // BRASSTACKS_EVENTS_WINDOWEVENT_HPP