#include "brasstacks/System/pch.hpp"
#include "brasstacks/Events/WindowEvent.hpp"

namespace btx {

WindowCloseEvent::WindowCloseEvent() :
    Event(EventType::WindowClosed)
{ }

} // namespace btx