#include "brasstacks/System/pch.hpp"
#include "brasstacks/Events/MouseEvent.hpp"

namespace btx {

std::pair<int32_t, int32_t> MouseMovedEvent::offset() const {
    return std::make_pair(_x_offset, _y_offset);
}

std::pair<int32_t, int32_t> MouseScrolledEvent::offset() const {
    return std::make_pair(_x_offset, _y_offset);
}

MouseMovedEvent::MouseMovedEvent(int32_t x_offset, int32_t y_offset) :
    Event(EventType::MouseMoved),
    _x_offset{ x_offset },
    _y_offset{ y_offset }
{ }

MouseScrolledEvent::MouseScrolledEvent(int32_t x_offset, int32_t y_offset) :
    Event(EventType::MouseScrolled),
    _x_offset{ x_offset },
    _y_offset{ y_offset }
{ }

MouseButtonPressedEvent::MouseButtonPressedEvent(Keycode button) :
    Event(EventType::MouseButtonPressed),
    _button{ button }
{ }

MouseButtonReleasedEvent::MouseButtonReleasedEvent(Keycode button) :
    Event(EventType::MouseButtonReleased),
    _button{ button }
{ }

} // namespace btx