#include "brasstacks/events/EventBus.hpp"

namespace btx {

std::vector<std::vector<EventBus::PushFn>> EventBus::_queues_by_event { };

} // namespace btx