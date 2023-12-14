#include "brasstacks/events/EventBroker.hpp"

namespace btx {

EventBroker::CallbackList EventBroker::_callbacks;
uint32_t EventBroker::_next_event_id = 0u;

} // namespace btx