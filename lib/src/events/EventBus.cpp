#include "brasstacks/brasstacks.hpp"
#include "brasstacks/events/EventBus.hpp"

namespace btx {

std::vector<EventBus::QueueCallbacks> EventBus::_callbacks_by_event { };

} // namespace btx