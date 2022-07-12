#ifndef BRASSTACKS_EVENTS_MOUSEEVENT_HPP
#define BRASSTACKS_EVENTS_MOUSEEVENT_HPP

#include "brasstacks/Events/Event.hpp"
#include "brasstacks/System/Input/KeyCodes.hpp"

namespace btx {

class MouseMovedEvent : public Event {
public:
	std::pair<int32_t, int32_t> offset() const;

	MouseMovedEvent(int32_t x_offset, int32_t y_offset);

private:
	int32_t _x_offset;
	int32_t _y_offset;
};

class MouseScrolledEvent : public Event {
public:
	std::pair<int32_t, int32_t> offset() const;

	MouseScrolledEvent(int32_t x_offset, int32_t y_offset);

private:
	int32_t _x_offset;
	int32_t _y_offset;
};

class MouseButtonPressedEvent : public Event {
public:
    Keycode button() const { return _button; }

	MouseButtonPressedEvent(Keycode button);

private:
	Keycode _button;
};

class MouseButtonReleasedEvent : public Event {
public:
    Keycode button() const { return _button; }

	MouseButtonReleasedEvent(Keycode button);

private:
	Keycode _button;
};


} // namespace btx

#endif // BRASSTACKS_EVENTS_MOUSEEVENT_HPP