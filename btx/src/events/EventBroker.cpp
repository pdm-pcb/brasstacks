#include "brasstacks/events/EventBroker.hpp"

namespace btx {

EventBroker::CallbackList EventBroker::_callbacks;
uint32_t EventBroker::_next_event_id = 0u;

// =============================================================================
void EventBroker::unsubscribe(const EventListenerHandle &handle) {
    _callbacks[handle.event_id]->remove(handle);
}

// =============================================================================
void EventBroker::init() {
    _callbacks.reserve(EventCallbacksBase::MAX_CALLBACKS);
}

// =============================================================================
void EventBroker::shutdown() {
    for(auto const *callback : _callbacks) {
        delete callback;
    }
}

} // namespace btx