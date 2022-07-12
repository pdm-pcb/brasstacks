#ifndef BRASSTACKS_EVENTS_EVENTBASE_HPP
#define BRASSTACKS_EVENTS_EVENTBASE_HPP

#include <string>

namespace btx {

enum class EventType {
	WindowClosed,
	KeyPressed, KeyReleased,
	MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,

	END
};

class Event {
public:
    EventType type() { return _type; }

protected:
    Event(const EventType type) : _type { type }  { }

private:
	EventType _type;
};

} // namespace btx

#endif // BRASSTACKS_EVENTS_EVENTBASE_HPP