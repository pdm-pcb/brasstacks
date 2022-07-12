#include "brasstacks/System/pch.hpp"
#include "brasstacks/Events/KeyboardEvent.hpp"

namespace btx {

KeyPressedEvent::KeyPressedEvent(Keycode keycode) :
    Event(EventType::KeyPressed),
    _keycode { keycode }
{ }

KeyReleasedEvent::KeyReleasedEvent(Keycode keycode) :
    Event(EventType::KeyReleased),
    _keycode { keycode }
{ }

} // namespace btx