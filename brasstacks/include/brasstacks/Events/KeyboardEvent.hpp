#ifndef BRASSTACKS_EVENTS_KEYEVENT_HPP
#define BRASSTACKS_EVENTS_KEYEVENT_HPP

#include "brasstacks/Events/Event.hpp"
#include "brasstacks/System/Input/KeyCodes.hpp"

namespace btx {

class KeyPressedEvent : public Event {
public:
	Keycode key() const { return _keycode; }

	explicit KeyPressedEvent(Keycode keycode);
    KeyPressedEvent()  = delete;
	~KeyPressedEvent() = default;

	KeyPressedEvent(const KeyPressedEvent &&) = delete;
	KeyPressedEvent(KeyPressedEvent &)        = delete;

	KeyPressedEvent & operator=(const KeyPressedEvent &&) = delete;
	KeyPressedEvent & operator=(KeyPressedEvent &)        = delete;

private:
	Keycode _keycode;
};

class KeyReleasedEvent : public Event {
public:
	Keycode key() const { return _keycode; }

	explicit KeyReleasedEvent(Keycode keycode);
    KeyReleasedEvent()  = delete;
	~KeyReleasedEvent() = default;

	KeyReleasedEvent(const KeyReleasedEvent &&) = delete;
	KeyReleasedEvent(KeyReleasedEvent &)        = delete;

	KeyReleasedEvent & operator=(const KeyReleasedEvent &&) = delete;
	KeyReleasedEvent & operator=(KeyReleasedEvent &)        = delete;

private:
	Keycode _keycode;
};

} // namespace btx

#endif // BRASSTACKS_EVENTS_KEYEVENT_HPP